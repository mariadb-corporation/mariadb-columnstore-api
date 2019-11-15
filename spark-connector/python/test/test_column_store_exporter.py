# Copyright (c) 2018, MariaDB Corporation. All rights reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA

import pytest, sys, os, datetime, decimal, columnStoreExporter, platform
from pyspark.sql import SparkSession, Row
import mysql.connector as mariadb

#verify that the dataframe was stored correctly
def verifyAllTypes(conn, table, rowid, expected):
    query_all_types = "select uint64, int64, uint32, int32, uint16, int16, uint8, `int8`, f, d, ch4, vch30, dt, dtm, dc, tx, bit, mathInt, dc2 from %s where uint64 = %s" % (table, rowid)
    try:
        cursor = conn.cursor()
        cursor.execute(query_all_types)
        resultsFound = False
        for (uint64, int64, uint32, int32, uint16, int16, uint8, int8, f, d, ch4, vch30, dt, dtm, dc, tx, bit, mathInt, dc2) in cursor:
            resultsFound = True
            rowStr = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}".format(uint64, int64, uint32, int32, uint16, int16, uint8, int8, f, d, ch4, vch30, dt, dtm, dc, tx, bit, mathInt, dc2)
            assert rowStr == expected
    except mariadb.Error as err:
        pytest.fail("Error during test verification: %s" %(err))
    except AssertionError as e:
        pytest.fail("%s\nInjected doesn't match expetations.\nexpected: %s\nactual:   %s" % (e,expected,rowStr))
    finally:
        if cursor: cursor.close()
    if not resultsFound:
        pytest.fail("no data was injected into columnstore")

def test_all_types():
    #Datatype settings
    global long
    if sys.version_info[0] == 3:
        long = int
    
    #Get connection parameter
    user = "root"
    host = "localhost"
    if os.environ.get("MCSAPI_CS_TEST_IP") is not None:
        host=os.environ.get("MCSAPI_CS_TEST_IP")
    if os.environ.get("MCSAPI_CS_TEST_USER") is not None:
        user=os.environ.get("MCSAPI_CS_TEST_USER")
    password = os.environ.get("MCSAPI_CS_TEST_PASSWORD")
    
    #Create the spark session
    spark = SparkSession.builder.getOrCreate()
    
    #create the test table
    try:
        conn = mariadb.connect(user=user, database='test', host=host, password=password)
        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS pythontest")
        cursor.execute(" \
        CREATE TABLE pythontest ( \
        uint64 bigint unsigned, \
        int64 bigint, \
        uint32 int unsigned, \
        int32 int, \
        uint16 smallint unsigned, \
        int16 smallint, \
        uint8 tinyint unsigned, \
        `int8` tinyint, \
        f float, \
        d double, \
        ch4 char(5), \
        vch30 varchar(30), \
        dt date, \
        dtm datetime, \
        dc decimal(18), \
        tx text, \
        bit tinyint(1), \
        mathInt bigint unsigned, \
        dc2 decimal(18,9)) \
        ENGINE=columnstore")
        cursor.execute("DROP TABLE IF EXISTS pythontest2")
        cursor.execute(" \
        CREATE TABLE pythontest2 ( \
        uint64 bigint unsigned, \
        int64 bigint, \
        uint32 int unsigned, \
        int32 int, \
        uint16 smallint unsigned, \
        int16 smallint, \
        uint8 tinyint unsigned, \
        `int8` tinyint, \
        f float, \
        d double, \
        ch4 char(5), \
        vch30 varchar(30), \
        dt date, \
        dtm datetime, \
        dc decimal(18), \
        tx text, \
        bit tinyint(1), \
        mathInt bigint unsigned, \
        dc2 decimal(18,9)) \
        ENGINE=columnstore")
    
    except mariadb.Error as err:
        pytest.fail("Error creating table pythontest or pythontest2")
    finally:
        if cursor: cursor.close()
        if conn: conn.close()
    
    # create the test dataframe
    TestRowStructure = Row("uint64", "int64", "uint32", "int32", "uint16", "int16", "uint8", "int8", "f", "d", "ch4", "vch30", "dt", "dtm", "dc", "tx", "bit", "mathInt", "dc2")
    testCase1 = TestRowStructure(long(1), long(2), long(3), int(4), int(5), int(6), int(7), int(8), float(1.234), float(2.34567), "ABCD", "Hello World", datetime.date(2017,9,8), datetime.datetime(2017,9,8,13,58,23), decimal.Decimal(123), "Hello World Longer", True, decimal.Decimal("9223372036854775807"), decimal.Decimal("-0.000000001"))
    if sys.version_info[0] == 2 or platform.system() == "Windows":
        testCase2 = TestRowStructure(long(0), long(-9223372036854775806), long(0), int(-2147483646), int(0), int(-32766), int(0), int(-126), float(1.234), float(2.34567), "A", "B", datetime.date(2000,1,1), datetime.datetime(2000,1,1,0,0,0), decimal.Decimal(-123), "C", False, decimal.Decimal("18446744073709551613"), decimal.Decimal("100000000.999999999"))
    else:
        testCase2 = TestRowStructure(long(0), long(-9223372036854775806), long(0), int(-2147483646), int(0), int(-32766), int(0), int(-126), float(1.234), float(2.34567), "A", "B", datetime.date(1000,1,1), datetime.datetime(1000,1,1,0,0,0), decimal.Decimal(-123), "C", False, decimal.Decimal("18446744073709551613"), decimal.Decimal("100000000.999999999"))
    if platform.system() == "Windows":
        testCase3 = TestRowStructure(long(9223372036854775807), long(9223372036854775807), long(4294967293), int(2147483647), int(65533), int(32767), int(253), int(127), float(1.234), float(2.34567), "ZYXW", "012345678901234567890123456789", datetime.date(9999,12,31), datetime.datetime(2048,12,31,23,59,59), decimal.Decimal(123), "012345678901234567890123456789", True, decimal.Decimal("2342"), decimal.Decimal("23.42"))
    else:
        testCase3 = TestRowStructure(long(9223372036854775807), long(9223372036854775807), long(4294967293), int(2147483647), int(65533), int(32767), int(253), int(127), float(1.234), float(2.34567), "ZYXW", "012345678901234567890123456789", datetime.date(9999,12,31), datetime.datetime(9999,12,31,23,59,59), decimal.Decimal(123), "012345678901234567890123456789", True, decimal.Decimal("2342"), decimal.Decimal("23.42"))
    testCase4 = TestRowStructure(42, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None)

    testCaseSeq = [testCase1, testCase2, testCase3, testCase4]
    testDF = spark.createDataFrame(testCaseSeq)
    
    #write the test dataframe into columnstore
    columnStoreExporter.export("test","pythontest",testDF)
    if os.environ.get("COLUMNSTORE_INSTALL_DIR") is not None:
        columnStoreExporter.export("test","pythontest2",testDF,os.environ.get("COLUMNSTORE_INSTALL_DIR")+"/etc/Columnstore.xml")
    else:
        columnStoreExporter.export("test","pythontest2",testDF,"/etc/columnstore/Columnstore.xml")
    
    #verify that the dataframe was stored correctly
    tables = ['pythontest', 'pythontest2']
    for table in tables:
        connection = mariadb.connect(user=user, database='test', host=host, password=password)
        verifyAllTypes(connection, table, 1, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23, 123, Hello World Longer, 1, 9223372036854775807, -1E-9")
        if sys.version_info[0] == 2 or platform.system() == "Windows":
            verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 2000-01-01, 2000-01-01 00:00:00, -123, C, 0, 18446744073709551613, 100000000.999999999")
        else:
            verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00, -123, C, 0, 18446744073709551613, 100000000.999999999")
        if platform.system() == "Windows":
            verifyAllTypes(connection, table, 9223372036854775807, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 2048-12-31 23:59:59, 123, 012345678901234567890123456789, 1, 2342, 23.420000000")
        else:
            verifyAllTypes(connection, table, 9223372036854775807, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59, 123, 012345678901234567890123456789, 1, 2342, 23.420000000")
        verifyAllTypes(connection, table, 42, "42, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None, None")
    
    #drop the test tables
    cursor = connection.cursor()
    cursor.execute("DROP TABLE IF EXISTS pythontest")
    cursor.execute("DROP TABLE IF EXISTS pythontest2")
    cursor.close()
    connection.close()


