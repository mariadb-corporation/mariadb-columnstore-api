#!/usr/bin/python

import os, sys, subprocess, datetime, yaml, csv, random, time
import mysql.connector as mariadb

DB_NAME = 'test'

# main test program
def executeTestSuite():
    if len(sys.argv) != 2:
        print(sys.argv[0], "mcsimport_executable")
        sys.exit(666)

    test_path = os.path.dirname(os.path.realpath(__file__))
    global mcsimport_executable
    mcsimport_executable = sys.argv[1]

    #check if the provided mcsimport_executable is valid
    try:
        subprocess.call([mcsimport_executable])
    except Exception as e:
        print("mcsimport couldn't be executed\n", e)
        sys.exit(667)
    
    # set up necessary variables
    global user
    user = "root"
    global host
    host = "localhost"
    if os.environ.get("MCSAPI_CS_TEST_IP") is not None:
        host=os.environ.get("MCSAPI_CS_TEST_IP")
    if os.environ.get("MCSAPI_CS_TEST_USER") is not None:
        user=os.environ.get("MCSAPI_CS_TEST_USER")
    global password
    password = os.environ.get("MCSAPI_CS_TEST_PASSWORD")

    # test execution main loop
    print("")
    total_tests = 0
    failed_tests = []
    for file in os.listdir(test_path):
        if (os.path.isdir(os.path.join(test_path, file))):
            total_tests +=1
            test_directory = os.path.join(test_path, file)
            failed = executeTest(test_directory)
            if failed:
                failed_tests.append(file)
    
    print("\n%d tests failed out of %d" %(len(failed_tests),total_tests))
    if len(failed_tests) > 0:
        print("\nfailed tests:")
        for t in failed_tests:
            print("- %s" % (t,))
    
    sys.exit(len(failed_tests))

# executes a test of a sub-directory and returns True if the test failed
def executeTest(test_directory):
    failed = False
    print("Execute test from directory %s" %(test_directory,))
    
    # load the test configuration
    try:
        testConfig = loadTestConfig(test_directory)
        print("Configuration loaded for test: %s" % (testConfig["name"]))
    except Exception as e:
        print("Error while processing test configuration %s\nError: %s\nTest failed\n" %(os.path.join(test_directory,"config.yaml"),e))
        return True
    
    # execute the prepare_test method to generate needed test files if prepare.py is present in the test directory
    if os.path.exists(os.path.join(test_directory,"prepare.py")):
        sys.path.append(test_directory)
        try:
            import prepare
            prepare = reload(prepare)
            prepare.prepare_test(test_directory)
        except Exception as e:
            print("Error during the processing of %s.\nError: %s\nTest failed\n" %(os.path.join(test_directory,"prepare.py"),e))
            failed = True
        finally:
            sys.path.remove(test_directory)
        if failed:
            return True
        else:
            print("Executed custom prepare_test() method")    
    
    # execute the DDL to prepare the test table if DDL.sql is present in test directory
    if os.path.exists(os.path.join(test_directory,"DDL.sql")):
        failed = prepareColumnstoreTable(os.path.join(test_directory,"DDL.sql"),testConfig["table"])
        if failed:
            print("Test failed\n")
            return True
        else:
            print("Test target tables created")
    
    # do the actual injection via mcsimport
    t = time.time()
    failed = executeMcsimport(test_directory,testConfig)
    print("mcsimport execution time: %ds" % (time.time() - t))
    if failed:
        print("Test failed\n")
        return True
    else:
        print("mcsimport executed and return code validated")
    
    # validate the test results line by line if expected.csv is found
    if os.path.exists(os.path.join(test_directory,"expected.csv")):
        t = time.time()
        failed = validateInjection(test_directory,testConfig["table"],testConfig["validation_coverage"])
        if failed:
            print("validation time: %ds" % (time.time() - t))
            print("Test failed\n")
            return True
        else:
            if testConfig["validation_coverage"] is None:
                print("Injection validated successfull against expected.csv")
            elif testConfig["validation_coverage"] == 0:
                print("Row count of injected columnstore table validated against the row count of expected.csv")
            else:
                print("Injection validated successfull against expected.csv with a coverage of %s%%" % (str(testConfig["validation_coverage"]),))
        print("validation time: %ds" % (time.time() - t))
    
    # clean up generated input files through prepare.py's cleanup_test method
    if os.path.exists(os.path.join(test_directory,"prepare.py")):
        try:
            prepare.cleanup_test(test_directory)
        except Exception as e:
            print("Error during the processing of %s.\nError: %s\nTest failed\n" %(os.path.join(test_directory,"prepare.py"),e))
            return True
        print("Executed custom cleanup_test() method")
    
    # clean up the columnstore test table
    failed = cleanUpColumnstoreTable(testConfig["table"])
    if failed:
        print("Test failed\n")
        return True
    else:
        print("Test target tables cleaned up")
    
    
    print("Test succeeded\n")
    return False

# parses the configuration file, loads the test config, and throws an exception if it fails
def loadTestConfig(test_directory):
    with open(os.path.join(test_directory,"config.yaml"), 'r') as stream:
        testConfig = yaml.load(stream)
    if testConfig["name"] is None:
        raise Exception("test's name couldn't be extracted from configuration")
    if testConfig["expected_exit_value"] is None:
        raise Exception("test's expected exit value couldn't be extracted from configuration")
    if "validation_coverage" in testConfig:
        if not (testConfig["validation_coverage"] is None or (int(testConfig["validation_coverage"]) <=100 and int(testConfig["validation_coverage"]) >=0)):
            raise Exception("test's coverage value: %s is invalid" %(validation_coverage,))
    else:
        testConfig["validation_coverage"] = None
    if not "header" in testConfig:
        testConfig["header"] = False
    if not "null_option" in testConfig:
        testConfig["null_option"] = None
    if not "ignore_malformed_csv" in testConfig:
        testConfig["ignore_malformed_csv"] = None
    if not "enclosing_character" in testConfig:
        testConfig["enclosing_character"] = None
    if not "escaping_character" in testConfig:
        testConfig["escaping_character"] = None
    return testConfig
    
# executes the SQL statements of given file to set up the test table
def prepareColumnstoreTable(file, table):
    error = False
    try:
        conn = mariadb.connect(user=user, password=password, host=host, database=DB_NAME)
        cursor = conn.cursor();
        cursor.execute("DROP TABLE IF EXISTS %s" %(table,))
        with open(file) as f:
            content = f.readlines()
            content = [x.strip() for x in content]
            for line in content:
                cursor.execute(line)
    except mariadb.Error as err:
        try:
            print("Error during SQL operation while processing %s.\nLine: %s\nError: %s" %(file,line,err))
        except NameError:
            print("Error during SQL operation while processing %s.\nError: %s" %(file,err))
        error = True
    except Exception as e:
        print("Error while processing %s.\nError: %s" %(file,e))
        error = True
    finally:
        try:
            if cursor: cursor.close()
            if conn: conn.close()
        except NameError:
            pass
    
    return error

# executes mcsimport and compares the expected return code. Returns True if the return code doesn't match.
def executeMcsimport(test_directory,testConfig):
    
    # forge the test command to execute
    cmd = [mcsimport_executable, DB_NAME, testConfig["table"]]
    if os.path.exists(os.path.join(test_directory,"input.csv")):
        cmd.append(os.path.join(test_directory,"input.csv"))
    if os.path.exists(os.path.join(test_directory,"mapping.yaml")):
        cmd.append("-m")
        cmd.append("%s" % (os.path.join(test_directory,"mapping.yaml"),))
    if os.path.exists(os.path.join(test_directory,"Columnstore.xml")):
        cmd.append("-c") 
        cmd.append("%s" % (os.path.join(test_directory,"Columnstore.xml"),))
    if testConfig["delimiter"] is not None:
        cmd.append("-d")
        cmd.append("%s" % (testConfig["delimiter"],))
    if testConfig["date_format"] is not None:
        cmd.append("-df")
        cmd.append("%s" % (testConfig["date_format"],))
    if testConfig["default_non_mapped"]:
        cmd.append("-default_non_mapped")
    if testConfig["header"]:
        cmd.append("-header")
    if testConfig["enclosing_character"] is not None:
        cmd.append("-E")
        cmd.append("%s" % (testConfig["enclosing_character"]))
    if testConfig["escaping_character"] is not None:
        cmd.append("-C")
        cmd.append("%s" % (testConfig["escaping_character"]))
    if testConfig["ignore_malformed_csv"]:
        cmd.append("-ignore_malformed_csv")
    if testConfig["null_option"] is not None:
        cmd.append("-n")
        cmd.append("%s" % (testConfig["null_option"]))
    
    print("Execute mcsimport: %s" % (cmd,))
    try:
        subprocess.check_output(cmd,stderr=subprocess.STDOUT,universal_newlines=True)
    except subprocess.CalledProcessError as exc: # this exception is raised once the return code is not 0
        if testConfig["expected_exit_value"] != exc.returncode: # therefore we have to validate mcsimport's return code here
            print("Error while executing mcsimport.\nmcsimport output: %s\nError: mcsimport's actual return code of %d doesn't match the expected return code of %d" %(exc.output, exc.returncode, testConfig["expected_exit_value"]))
            return True
        else:
            return False
    except Exception as e:
        print("Error while executing mcsimport.\nError: %s" %(e,))
        return True
    
    if testConfig["expected_exit_value"] != 0: # we further have to validate mcsimport's return code here in case it was 0 and didn't raise the exception
        print("Error while executing mcsimport.\nError: mcsimport's actual return code of 0 doesn't match the expected return code of %d" %(testConfig["expected_exit_value"],))
        return True
    
    return False

# validates if the injected values match the expected values
def validateInjection(test_directory,table,validationCoverage):
    error = False
    try:
        conn = mariadb.connect(user=user, password=password, host=host, database=DB_NAME)
        cursor = conn.cursor();
        # validate that the number of rows of expected.csv and target table match
        cursor.execute("SELECT COUNT(*) AS cnt FROM %s" % (table,))
        cnt = cursor.fetchone()[0]
        num_lines = 0
        with open(os.path.join(test_directory,"expected.csv")) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            for line in csv_reader:
                num_lines = num_lines+1
        assert num_lines == cnt, "the number of injected rows: %d doesn't match the number of expected rows: %d" % (cnt, num_lines)
        # validate that each input line in expected.csv was injected into the target table
        if validationCoverage != 0:
            with open(os.path.join(test_directory,"expected.csv")) as csv_file:
                csv_reader = csv.reader(csv_file, delimiter=',')
                for line in csv_reader:
                    if validationCoverage is None or random.randint(1,100) < int(validationCoverage): 
                        if line[0] != "":
                            cursor.execute("SELECT * FROM %s WHERE id=%s" % (table,line[0]))
                        else:
                            cursor.execute("SELECT * FROM %s WHERE id IS NULL" % (table,))
                        row = cursor.fetchone()
                        assert row is not None, "no target row could be fetched for expected id: %s" %(str(line[0]),)
                        
                        for i in range(len(line)):
                            if row[i] is None:
                                assert line[i] == "", "target and expected don't match.\ntarget:   %s\nexpected: %s\ntarget item: NULL doesn't match expected item: %s" % (row,line,str(line[i]))
                            else:
                                assert str(line[i]) == str(row[i]), "target and expected don't match.\ntarget:   %s\nexpected: %s\ntarget item: %s doesn't match expected item: %s" % (row,line,str(row[i]),str(line[i]))
    except mariadb.Error as err:
        print("SQL error during ColumnStore validation operation.\nError: %s" %(err,))
        error = True
    except AssertionError as er:
        print("Assertion error during ColumnStore validation operation.\nError: %s" %(er,))
        error = True
    except Exception as e:
        print("Error during ColumnStore validation operation.\nError: %s" %(e,))
        error = True
    finally:
        if cursor: cursor.close()
        if conn: conn.close()
    
    return error
    
# cleans up the ColumnStore test table
def cleanUpColumnstoreTable(table):
    error = False
    try:
        conn = mariadb.connect(user=user, password=password, host=host, database=DB_NAME)
        cursor = conn.cursor();
        cursor.execute("DROP TABLE IF EXISTS %s" %(table,))
    except mariadb.Error as err:
        print("Error during ColumnStore test table cleanup operation.\nError: %s" %(err,))
        error = True
    finally:
        if cursor: cursor.close()
        if conn: conn.close()
    
    return error
    
# execute the test suite
executeTestSuite()

