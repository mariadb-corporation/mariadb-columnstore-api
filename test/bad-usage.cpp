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
    my_con = mysql_init(NULL);
    if (!my_con)
        FAIL() << "Could not init MariaDB connection";
    if (!mysql_real_connect(my_con, "127.0.0.1", "root", "", NULL, 3306, NULL, 0))
        FAIL() << "Could not connect to MariaDB: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE DATABASE IF NOT EXISTS mcsapi"))
        FAIL() << "Error creating database: " << mysql_error(my_con);
    if (mysql_select_db(my_con, "mcsapi"))
        FAIL() << "Could not select DB: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS badusage"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS badusage (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE badusage"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Test that use after commit fails */
TEST(BadUsage, UseAfterCommit)
{
    std::string table("badusage");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        for (int i = 0; i < 1000; i++)
        {
        	bulk->setColumn(0, (uint32_t)i);
        	bulk->setColumn(1, (uint32_t)1000 - i);
        	bulk->writeRow();
        }
        bulk->commit();
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    ASSERT_THROW(bulk->setColumn(0, (uint32_t)1), mcsapi::ColumnStoreException);
    ASSERT_THROW(bulk->writeRow(), mcsapi::ColumnStoreException);
    if (mysql_query(my_con, "SELECT COUNT(*) FROM badusage"))
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

/* Test not all columns filled fails */
TEST(BadUsage, ColumnNotFilled)
{
    std::string table("badusage");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk->setColumn(0, (uint32_t)1);
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    ASSERT_THROW(bulk->writeRow(), mcsapi::ColumnStoreException);
    try {
        bulk->rollback();
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM badusage"))
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

/* Test invalid column ID fails */
TEST(BadUsage, InvalidColumn)
{
    std::string table("badusage");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    ASSERT_THROW(bulk->setColumn(3, (uint32_t)1), mcsapi::ColumnStoreException);
    try {
        bulk->rollback();
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM badusage"))
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

/* Test that second commit works */
TEST(BadUsage, SecondCommit)
{
    std::string table("badusage");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        for (int i = 0; i < 1000; i++)
        {
            bulk->setColumn(0, (uint32_t)i);
            bulk->setColumn(1, (uint32_t)1000 - i);
            bulk->writeRow();
        }
        bulk->commit();
    } catch (mcsapi::ColumnStoreException &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM badusage"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "2000");
    mysql_free_result(result);
    delete bulk;
    delete driver;
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
