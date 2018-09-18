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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS summary"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS summary (a int, b varchar(10)) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE summary"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Test that rows inserted works */
TEST(Summary, RowsInserted)
{
    std::string table("summary");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    mcsapi::columnstore_data_convert_status_t status;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        std::string sData("test");
        for (int i = 0; i < 1000; i++)
        {
            bulk->setColumn(0, (uint32_t)i, &status);
            ASSERT_EQ(status, mcsapi::CONVERT_STATUS_NONE);
            bulk->setColumn(1, sData);
            ASSERT_EQ(status, mcsapi::CONVERT_STATUS_NONE);
            bulk->writeRow();
        }
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    mcsapi::ColumnStoreSummary summary;
    summary = bulk->getSummary();
    ASSERT_EQ(summary.getRowsInsertedCount(), 1000);
    if (mysql_query(my_con, "SELECT COUNT(*) FROM summary"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000");
    mysql_free_result(result);
    delete bulk;
    delete driver;
}

/* Test that saturated works */
TEST(Summary, Saturated)
{
    std::string table("summary");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    mcsapi::columnstore_data_convert_status_t status;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk->setColumn(0, (uint32_t)0xFFFFFFFF, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_SATURATED);
        std::string sData("test");
        bulk->setColumn(1, sData, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_NONE);
        bulk->writeRow();
        bulk->rollback();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    mcsapi::ColumnStoreSummary summary;
    summary = bulk->getSummary();
    ASSERT_EQ(summary.getSaturatedCount(), 1);
    if (mysql_query(my_con, "SELECT COUNT(*) FROM summary"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000");
    mysql_free_result(result);
    delete bulk;
    delete driver;
}

/* Test that invalid works */
TEST(Summary, Invalid)
{
    std::string table("summary");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    mcsapi::columnstore_data_convert_status_t status;
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
        mcsapi::ColumnStoreDateTime tData;
        tData.set(sTm);
        bulk->setColumn(0, tData, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_INVALID);
        std::string sData = "test";
        bulk->setColumn(1, sData, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_NONE);
        bulk->writeRow();

        bulk->rollback();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    mcsapi::ColumnStoreSummary summary;
    summary = bulk->getSummary();
    ASSERT_EQ(summary.getInvalidCount(), 1);
    if (mysql_query(my_con, "SELECT COUNT(*) FROM summary"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000");
    mysql_free_result(result);
    delete driver;
}

/* Test that truncated works */
TEST(Summary, Truncated)
{
    std::string table("summary");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    mcsapi::columnstore_data_convert_status_t status;
    std::string sData("This is a longer string to trigger a truncation warning");
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk->setColumn(0, 9999, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_NONE);
        bulk->setColumn(1, sData, &status);
        ASSERT_EQ(status, mcsapi::CONVERT_STATUS_TRUNCATED);
        bulk->writeRow();
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    mcsapi::ColumnStoreSummary summary;
    summary = bulk->getSummary();
    ASSERT_EQ(summary.getTruncationCount(), 1);
    if (mysql_query(my_con, "SELECT COUNT(*) FROM summary"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1001");
    mysql_free_result(result);
    delete bulk;
    delete driver;
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
