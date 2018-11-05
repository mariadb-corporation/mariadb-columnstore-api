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
#include <mysql.h>

MYSQL* my_con;

class TestEnvironment : public ::testing::Environment {
 public:
  virtual ~TestEnvironment() {}
  // Override this to define how to set up the environment.
  virtual void SetUp()
  {
    std::string cs_ip = "127.0.0.1";
    std::string cs_user = "root";
    std::string cs_password = "";
    if(std::getenv("MCSAPI_CS_TEST_IP")){
        cs_ip = std::getenv("MCSAPI_CS_TEST_IP");
    }
    if(std::getenv("MCSAPI_CS_TEST_USER")){
        cs_user = std::getenv("MCSAPI_CS_TEST_USER");
    }
    if(std::getenv("MCSAPI_CS_TEST_PASSWORD")){
        cs_password = std::getenv("MCSAPI_CS_TEST_PASSWORD");
    }
    my_con = mysql_init(NULL);
    if (!my_con)
        FAIL() << "Could not init MariaDB connection";
    if (!mysql_real_connect(my_con, cs_ip.c_str(), cs_user.c_str(), cs_password.c_str(), NULL, 3306, NULL, 0))
        FAIL() << "Could not connect to MariaDB: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE DATABASE IF NOT EXISTS mcsapi"))
        FAIL() << "Error creating database: " << mysql_error(my_con);
    if (mysql_select_db(my_con, "mcsapi"))
        FAIL() << "Could not select DB: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS dataconverttime"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS dataconverttime (a int, b varchar(50), c time, d time(6), e datetime) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        mysql_close(my_con);
    }
  }
};


/* Test that dataconvert from time works */
TEST(DataConvertTime, DataConvertTime)
{
    std::string table("dataconverttime");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        tm sTm;
        sTm.tm_year = 2017;
        // tm.mon is 0-11
        sTm.tm_mon = 5;
        sTm.tm_mday = 28;
        sTm.tm_hour = 13;
        sTm.tm_min = 28;
        sTm.tm_sec = 47;
        mcsapi::ColumnStoreTime tData;
        tData.set(sTm);
        bulk->setColumn(0, (uint64_t) 1);
        bulk->setColumn(1, tData);
        bulk->setColumn(2, tData);
        bulk->setColumn(3, tData);
        bulk->setColumn(4, tData);
        bulk->writeRow();
        std::string newTime("23:23:23");
        std::string tFormat("%H:%M:%S");
        tData.set(newTime, tFormat);
        bulk->setColumn(0, (uint64_t) 2);
        bulk->setColumn(1, tData);
        bulk->setColumn(2, tData);
        bulk->setColumn(3, tData);
        bulk->setColumn(4, tData);
        bulk->writeRow();
        mcsapi::ColumnStoreTime tDataz;
        bulk->setColumn(0, (uint64_t) 3);
        bulk->setColumn(1, tDataz);
        bulk->setColumn(2, tDataz);
        bulk->setColumn(3, tDataz);
        bulk->setColumn(4, tDataz);
        bulk->writeRow();
        mcsapi::ColumnStoreTime tp1(-838, 29, 29, 123456);
        bulk->setColumn(0, (uint64_t) 4);
        bulk->setColumn(1, tp1);
        bulk->setColumn(2, tp1);
        bulk->setColumn(3, tp1);
        bulk->setColumn(4, tp1);
        bulk->writeRow();
        mcsapi::ColumnStoreTime tp2(-0, 12, 12, 0, true);
        bulk->setColumn(0, (uint64_t) 5);
        bulk->setColumn(1, tp2);
        bulk->setColumn(2, tp2);
        bulk->setColumn(3, tp2);
        bulk->setColumn(4, tp2);
        bulk->writeRow();
        mcsapi::ColumnStoreTime tp3(0, 1, 15, 42, false);
        bulk->setColumn(0, (uint64_t) 6);
        bulk->setColumn(1, tp3);
        bulk->setColumn(2, tp3);
        bulk->setColumn(3, tp3);
        bulk->setColumn(4, tp3);
        bulk->writeRow();
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        if (bulk) bulk->rollback();
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT * FROM dataconverttime"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    ASSERT_EQ(mysql_num_rows(result), 6);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1");
    ASSERT_STREQ(row[1], "13:28:47");
    ASSERT_STREQ(row[2], "13:28:47");
    ASSERT_STREQ(row[3], "13:28:47.000000");
    ASSERT_STREQ(row[4], "0000-00-00 13:28:47");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "2");
    ASSERT_STREQ(row[1], "23:23:23");
    ASSERT_STREQ(row[2], "23:23:23");
    ASSERT_STREQ(row[3], "23:23:23.000000");
    ASSERT_STREQ(row[4], "0000-00-00 23:23:23");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "3");
    ASSERT_STREQ(row[1], "00:00:00");
    ASSERT_STREQ(row[2], "00:00:00");
    ASSERT_STREQ(row[3], "00:00:00.000000");
    ASSERT_STREQ(row[4], "0000-00-00 00:00:00");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "4");
    ASSERT_STREQ(row[1], "-838:29:29");
    ASSERT_STREQ(row[2], "-838:29:29");
    ASSERT_STREQ(row[3], "-838:29:29.123456");
    ASSERT_STREQ(row[4], "0000-00-00 00:29:29");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "5");
    ASSERT_STREQ(row[1], "-00:12:12");
    ASSERT_STREQ(row[2], "-00:12:12");
    ASSERT_STREQ(row[3], "-00:12:12.000000");
    ASSERT_STREQ(row[4], "0000-00-00 00:12:12");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "6");
    ASSERT_STREQ(row[1], "00:01:15");
    ASSERT_STREQ(row[2], "00:01:15");
    ASSERT_STREQ(row[3], "00:01:15.000042");
    ASSERT_STREQ(row[4], "0000-00-00 00:01:15");
    mysql_free_result(result);
    if (mysql_query(my_con, "DROP TABLE dataconverttime"))
        FAIL() << "Could not drop table: " << mysql_error(my_con);
    delete bulk;
    delete driver;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
