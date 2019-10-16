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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS commit"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS commit (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS commit2"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS commit2 (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE commit"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        if (mysql_query(my_con, "DROP TABLE commit2"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Test that commit works */
TEST(Commit, Commit)
{
    std::string table("commit");
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
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM commit"))
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

/* Test that rollback works */
TEST(Commit, Rollback)
{
    std::string table("commit");
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
        bulk->rollback();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM commit"))
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

/* Test that auto rollback works */
TEST(Commit, AutoRollback)
{
    std::string table("commit");
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
        delete bulk;
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM commit"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1000");
    mysql_free_result(result);
    delete driver;
}

/* Test that second commit works */
TEST(Commit, SecondCommit)
{
    std::string table("commit");
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
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM commit"))
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

/* Test that two transactions work */
TEST(Commit, TwoTransactions)
{
    std::string table("commit");
    std::string table2("commit2");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    mcsapi::ColumnStoreBulkInsert* bulk2;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk2 = driver->createBulkInsert(db, table2, 0, 0);
        for (int i = 0; i < 1000; i++)
        {
            bulk->setColumn(0, (uint32_t)i);
            bulk->setColumn(1, (uint32_t)1000 - i);
            bulk->writeRow();
            bulk2->setColumn(0, (uint32_t)i);
            bulk2->setColumn(1, (uint32_t)1000 - i);
            bulk2->writeRow();
        }
        bulk->commit();
        bulk2->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete bulk2;
    delete bulk;
    delete driver;
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
