/* Copyright (c) 2017, MariaDB Corporation. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */

#include <libmcsapi/mcsapi.h>
#include <iostream>
#include <gtest/gtest.h>
#include "common.h"

MYSQL* my_con;

class TestEnvironment : public ::testing::Environment {
 public:
  virtual ~TestEnvironment() {}
  // Override this to define how to set up the environment.
  virtual void SetUp()
  {
    my_con = get_mariadb();
    if (!my_con)
        FAIL() << "MariaDB connection setup failed";
    if (mysql_query(my_con, "DROP TABLE IF EXISTS dataconvertdecimal"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS dataconvertdecimal (a int, b varchar(50), c decimal(10,4), d double) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS dataconvertdecimal2"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS dataconvertdecimal2 (a int, b varchar(50), c decimal(18,9), d double) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE dataconvertdecimal"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        if (mysql_query(my_con, "DROP TABLE dataconvertdecimal2"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Test that dataconvert from decimal works */
TEST(DataConvertDecimal, DataConvertDecimal)
{
    std::string table("dataconvertdecimal");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        mcsapi::ColumnStoreDecimal dData;
        dData.set((int64_t)1000);
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set(11234567, 2);
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set((double)3.1415926);
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        mcsapi::ColumnStoreDecimal dDataz;
        bulk->setColumn(0, dDataz);
        bulk->setColumn(1, dDataz);
        bulk->setColumn(2, dDataz);
        bulk->setColumn(3, dDataz);
        bulk->writeRow();
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT * FROM dataconvertdecimal"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    ASSERT_EQ(mysql_num_rows(result), 4);
    MYSQL_ROW row = mysql_fetch_row(result);
    double dblval;
    std::string strval;
    ASSERT_STREQ(row[0], "1000");
    ASSERT_STREQ(row[1], "1000");
    ASSERT_STREQ(row[2], "1000.0000");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 1000);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "112346");
    ASSERT_STREQ(row[1], "112345.67");
    ASSERT_STREQ(row[2], "112345.6700");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 112345.67);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "3");
    strval = row[1];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 3.1415926);
    strval = row[2];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 3.1415);
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 3.1415926);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "0");
    ASSERT_STREQ(row[1], "0");
    ASSERT_STREQ(row[2], "0.0000");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 0);
    mysql_free_result(result);
    delete bulk;
    delete driver;
}

/* Test that dataconvert issues in MCOL-1133 */
TEST(DataConvertDecimal, MCOL1133)
{
    std::string table("dataconvertdecimal2");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        mcsapi::ColumnStoreDecimal dData;
        dData.set("100000000.999999000000000");
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set("-23.42");
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set("999999999.999999999"); //max value check
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set("1000000000.1234567890123"); //overflow check 
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set("999999999.999999999999999"); //overflow check
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        dData.set("-0.0001"); // negative below 1 check
        bulk->setColumn(0, dData);
        bulk->setColumn(1, dData);
        bulk->setColumn(2, dData);
        bulk->setColumn(3, dData);
        bulk->writeRow();
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT * FROM dataconvertdecimal2"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    ASSERT_EQ(mysql_num_rows(result), 6);
    MYSQL_ROW row = mysql_fetch_row(result);
    double dblval;
    std::string strval;
    ASSERT_STREQ(row[0], "100000001");
    ASSERT_STREQ(row[1], "100000000.999999");
    ASSERT_STREQ(row[2], "100000000.999999000");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 100000000.999999000);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "-23");
    ASSERT_STREQ(row[1], "-23.42");
    ASSERT_STREQ(row[2], "-23.420000000");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, -23.42);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000000000");
    ASSERT_STREQ(row[1], "999999999.999999999");
    ASSERT_STREQ(row[2], "999999999.999999999");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 999999999.999999999);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000000000");
    ASSERT_STREQ(row[1], "1000000000.12345679");
    ASSERT_STREQ(row[2], "999999999.999999999");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 1000000000.1234567890123);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000000000");
    ASSERT_STREQ(row[1], "1000000000");
    ASSERT_STREQ(row[2], "999999999.999999999");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 999999999.999999999999999);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "0");
    ASSERT_STREQ(row[1], "-0.0001");
    ASSERT_STREQ(row[2], "-0.000100000");
    strval = row[3];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, -0.0001);
    mysql_free_result(result);
    delete bulk;
    delete driver;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
