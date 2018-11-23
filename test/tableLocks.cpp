/* Copyright (c) 2018, MariaDB Corporation. All rights reserved.
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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS tableLock1"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS tableLock2"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS tableLock1 (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS tableLock2 (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE tableLock1"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        if (mysql_query(my_con, "DROP TABLE tableLock2"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Tests if listTableLocks() detects locked tables*/
TEST(tableLocks, listTableLocks)
{
    mcsapi::ColumnStoreDriver* driver = nullptr;
    mcsapi::ColumnStoreBulkInsert* bulk1 = nullptr;
    mcsapi::ColumnStoreBulkInsert* bulk2 = nullptr;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        uint64_t lockedTablesAtBeginning = driver->listTableLocks().size();
        // verify that one additional table is locked
        bulk1 = driver->createBulkInsert("mcsapi", "tableLock1", 0, 0);
        ASSERT_EQ(1, driver->listTableLocks().size() - lockedTablesAtBeginning);
        //verify that two additional tables are locked
        bulk2 = driver->createBulkInsert("mcsapi", "tableLock2", 0, 0);
        ASSERT_EQ(2, driver->listTableLocks().size() - lockedTablesAtBeginning);
        // verify that one additional table is locked
        bulk2->rollback();
        ASSERT_EQ(1, driver->listTableLocks().size() - lockedTablesAtBeginning);
        // verify that no additional table is locked
        bulk1->rollback();
        ASSERT_EQ(0, driver->listTableLocks().size() - lockedTablesAtBeginning);

    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }

    delete bulk1;
    delete bulk2;
    delete driver;
}

/* Tests if isTableLocked() detects locked tables*/
TEST(tableLocks, isTableLocked)
{
    mcsapi::ColumnStoreDriver* driver = nullptr;
    mcsapi::ColumnStoreBulkInsert* bulk = nullptr;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        // verify that tableLock1 is locked
        bulk = driver->createBulkInsert("mcsapi", "tableLock1", 0, 0);
        ASSERT_EQ(true, driver->isTableLocked("mcsapi", "tableLock1"));
        bulk->rollback();
        // verify that tableLock1 is not locked after rollback
        ASSERT_EQ(false, driver->isTableLocked("mcsapi", "tableLock1"));
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }

    delete bulk;
    delete driver;
}

/* Tests if clearTableLock() releases locked tables*/
TEST(tableLocks, clearTableLock)
{
    mcsapi::ColumnStoreDriver* driver = nullptr;
    mcsapi::ColumnStoreBulkInsert* bulk1 = nullptr;
    mcsapi::ColumnStoreBulkInsert* bulk2 = nullptr;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        // initiate ingestion and verify that tableLock1 is locked
        bulk1 = driver->createBulkInsert("mcsapi", "tableLock1", 0, 0);
        for (int i = 0; i < 100001; i++) {
            bulk1->setColumn(0, 0);
            bulk1->setColumn(1, 1);
            bulk1->writeRow();
        }
        ASSERT_EQ(true, driver->isTableLocked("mcsapi", "tableLock1"));
        driver->clearTableLock("mcsapi", "tableLock1");
        // verify that tableLock1 is not locked after clearTableLock
        ASSERT_EQ(false, driver->isTableLocked("mcsapi", "tableLock1"));
        // use bulk 2 to ingest one row
        bulk2 = driver->createBulkInsert("mcsapi", "tableLock1", 0, 0);
        bulk2->setColumn(0, 23);
        bulk2->setColumn(1, 42);
        bulk2->writeRow();
        bulk2->commit();
        // verify that tableLock1 is not locked after commit of bulk2
        ASSERT_EQ(false, driver->isTableLocked("mcsapi", "tableLock1"));
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }

    // verify that only one row was written from bulk2 and bulk1's write was rolled back
    if (mysql_query(my_con, "SELECT COUNT(*) FROM tableLock1"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1");
    mysql_free_result(result);
    if (mysql_query(my_con, "SELECT b FROM tableLock1 WHERE a=23"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "42");
    mysql_free_result(result);

    delete bulk2;
    delete driver;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
