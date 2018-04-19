# coding=utf-8
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

import pytest, sys, datetime, decimal, columnStoreExporter
from pyspark.sql import SparkSession, Row
import mysql.connector as mariadb

#parseTableColumnNameToCSConvention test cases
#valid table and column names
def test_valid_table_column_name_to_cs_convention_parsing():
    validTableColumnNames = ["hello", "testing_21", "i_like_columnstore", "thisIsFun", "aBcDe", "AbcdEF"]
    for tableColumnName in validTableColumnNames:
        parsed = columnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
        try:
            assert parsed == tableColumnName
        except AssertionError as e:
             pytest.fail("Original valid table/column name: '%s' doesn't match parsed table name: '%s'" % (tableColumnName, parsed))

#reserved words will get a postfix
def test_reserved_word_parsing():
    reservedWords = ["integer", "int", "COLUmN", "DATABASE", "DOUBLE", "MASTER_HEARTBEAT_PERIOD"]
    for tableColumnName in reservedWords:
        parsed = columnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
        try:
            assert not parsed == tableColumnName
        except AssertionError as e:
             pytest.fail("Original reserved table/column name: '%s' does match parsed table name: '%s'" % (tableColumnName, parsed))
        doubleParsed = columnStoreExporter.parseTableColumnNameToCSConvention(parsed)
        try:
            assert parsed == doubleParsed
        except AssertionError as e:
             pytest.fail("Parsed valid table/column name: '%s' doesn't match double parsed table name: '%s'" % (parsed, doubleParsed))

#invalid characters will be exchanged with prefix or _
def test_invalid_word_parsing():
    invalidWords = ["öl", "roHöl", "_hello*&@PKJEk", "lksf3823DMO[]", "l3mo()ntree", "An@ther"]
    for tableColumnName in invalidWords:
        parsed = columnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
        try:
            assert not parsed == tableColumnName
        except AssertionError as e:
             pytest.fail("Original reserved table/column name: '%s' does match parsed table name: '%s'" % (tableColumnName, parsed))
        doubleParsed = columnStoreExporter.parseTableColumnNameToCSConvention(parsed)
        try:
            assert parsed == doubleParsed
        except AssertionError as e:
             pytest.fail("Parsed valid table/column name: '%s' doesn't match double parsed table name: '%s'" % (parsed, doubleParsed))

#too long name truncation test
def test_too_long_word_parsing():
    tooLongWords = ["aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab","ööööööööööööööööööööööööööööööööööööööööööööööööööööööööööööööö"]
    for tableColumnName in tooLongWords:
        parsed = columnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
        try:
            assert not parsed == tableColumnName
        except AssertionError as e:
             pytest.fail("Original reserved table/column name: '%s' does match parsed table name: '%s'" % (tableColumnName, parsed))
        doubleParsed = columnStoreExporter.parseTableColumnNameToCSConvention(parsed)
        try:
            assert parsed == doubleParsed
        except AssertionError as e:
             pytest.fail("Parsed valid table/column name: '%s' doesn't match double parsed table name: '%s'" % (parsed, doubleParsed))


#generateTableStatement test cases
#verify that the dataframe was stored correctly
def verifyAllTypes(conn, table, rowid, expected):
    query_all_types = "select uint64, int64, uint32, int32, uint16, int16, uint8, int8_rw, f, d, ch4, vch30, dt, dtm, dc, tx, bit_rw, mathInt, dc2 from %s where uint64 = %s" % (table, rowid)
    try:
        cursor = conn.cursor()
        cursor.execute(query_all_types)
        for (uint64, int64, uint32, int32, uint16, int16, uint8, int8_rw, f, d, ch4, vch30, dt, dtm, dc, tx, bit_rw, mathInt, dc2) in cursor:
            rowStr = "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}".format(uint64, int64, uint32, int32, uint16, int16, uint8, int8_rw, f, d, ch4, vch30, dt, dtm, dc, tx, bit_rw, mathInt, dc2)
            assert rowStr == expected
    except mariadb.Error as err:
        pytest.fail("Error executing query: %s, error: %s" %(verifyAllTypes,err))
    except AssertionError as e:
        print(rowStr)
        print(expected)
        pytest.fail("Error executing query: %s, error: %s" %(verifyAllTypes,e))
    finally:
        if cursor: cursor.close()

#test the sql generation by repeating the test_all case with the generated table
def test_sql_generation():
    #Datatype settings
    global long
    if sys.version_info[0] == 3:
        long = int
    
    #Create the spark session
    spark = SparkSession.builder.getOrCreate()
    
    # create the test dataframe
    TestRowStructure = Row("uint64", "int64", "uint32", "int32", "uint16", "int16", "uint8", "int8", "f", "d", "ch4", "vch30", "dt", "dtm", "dc", "tx", "bit", "mathInt", "dc2")
    testCase1 = TestRowStructure(long(1), long(2), long(3), int(4), int(5), int(6), int(7), int(8), float(1.234), float(2.34567), "ABCD", "Hello World", datetime.date(2017,9,8), datetime.datetime(2017,9,8,13,58,23), decimal.Decimal(123), "Hello World Longer", True, decimal.Decimal("9223372036854775807"), decimal.Decimal("-0.000000001"))
    if sys.version_info[0] == 2:
        testCase2 = TestRowStructure(long(0), long(-9223372036854775806), long(0), int(-2147483646), int(0), int(-32766), int(0), int(-126), float(1.234), float(2.34567), "A", "B", datetime.date(2000,1,1), datetime.datetime(2000,1,1,0,0,0), decimal.Decimal(-123), "C", False, decimal.Decimal("18446744073709551613"), decimal.Decimal("100000000.999999999"))
    else:
        testCase2 = TestRowStructure(long(0), long(-9223372036854775806), long(0), int(-2147483646), int(0), int(-32766), int(0), int(-126), float(1.234), float(2.34567), "A", "B", datetime.date(1000,1,1), datetime.datetime(1000,1,1,0,0,0), decimal.Decimal(-123), "C", False, decimal.Decimal("18446744073709551613"), decimal.Decimal("100000000.999999999"))
    testCase3 = TestRowStructure(long(9223372036854775807), long(9223372036854775807), long(4294967293), int(2147483647), int(65533), int(32767), int(253), int(127), float(1.234), float(2.34567), "ZYXW", "012345678901234567890123456789", datetime.date(9999,12,31), datetime.datetime(9999,12,31,23,59,59), decimal.Decimal(123), "012345678901234567890123456789", True, decimal.Decimal("2342"), decimal.Decimal("23.42"))
    testCaseSeq = [testCase1, testCase2, testCase3]
    testDF = spark.createDataFrame(testCaseSeq)
    
    #create the test table
    try:
        conn = mariadb.connect(user='root', database='test', host='127.0.0.1')
        cursor = conn.cursor()
        cursor.execute("DROP TABLE IF EXISTS pythontest_gen_1")
        cursor.execute("DROP TABLE IF EXISTS pythontest_gen_2")
        createSQL1 = columnStoreExporter.generateTableStatement(testDF, "test", "pythontest_gen_1", True)
        createSQL2 = columnStoreExporter.generateTableStatement(testDF, "test", "pythontest_gen_2")
        cursor.execute(createSQL1)
        cursor.execute(createSQL2)
    
    except mariadb.Error as err:
        print(err)
        print(createSQL1)
        print(createSQL2)
        pytest.fail("Error creating table pythontest_gen_1 or pythontest_gen_2")
    finally:
        if cursor: cursor.close()
        if conn: conn.close()

    #write the test dataframe into columnstore
    columnStoreExporter.export("test","pythontest_gen_1",testDF)
    columnStoreExporter.export("test","pythontest_gen_2",testDF)
    
    #verify that the dataframe was stored correctly
    #pythontest_gen_1
    table = "pythontest_gen_1"
    connection = mariadb.connect(user='root', database='test', host='127.0.0.1')
    verifyAllTypes(connection, table, 1, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23, 123, Hello World Longer, 1, 9223372036854775807, -1E-9")
    if sys.version_info[0] == 2:
        verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 2000-01-01, 2000-01-01 00:00:00, -123, C, 0, 18446744073709551613, 100000000.999999999")
    else:
        verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00, -123, C, 0, 18446744073709551613, 100000000.999999999")
    verifyAllTypes(connection, table, 9223372036854775807, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59, 123, 012345678901234567890123456789, 1, 2342, 23.420000000")
 
    #pythontest_gen_2   
    table = "pythontest_gen_2"
    verifyAllTypes(connection, table, 1, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23, 123.000000000, Hello World Longer, 1, 999999999.999999999, -1E-9")
    if sys.version_info[0] == 2:
        verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 2000-01-01, 2000-01-01 00:00:00, -123.000000000, C, 0, 999999999.999999999, 100000000.999999999")
    else:
        verifyAllTypes(connection, table, 0, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00, -123.000000000, C, 0, 999999999.999999999, 100000000.999999999")
    verifyAllTypes(connection, table, 9223372036854775807, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59, 123.000000000, 012345678901234567890123456789, 1, 2342.000000000, 23.420000000")

    #drop the test tables
    cursor = connection.cursor()
    cursor.execute("DROP TABLE IF EXISTS pythontest_gen_1")
    cursor.execute("DROP TABLE IF EXISTS pythontest_gen_2")
    cursor.close()
    connection.close()

