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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS char4"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS char4 (i int, ch char(4)) engine=columnstore"))
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


/* Test for char4 corruption */
TEST(char4, char4)
{
    std::string table("char4");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        
        bulk->setColumn(0, (uint32_t)1);
        bulk->setColumn(1, "ABC");
        bulk->writeRow();
        bulk->setColumn(0, (uint32_t)2);
        bulk->setColumn(1, "DEF");
        bulk->writeRow();
        bulk->setColumn(0, (uint32_t)3);
        bulk->setColumn(1, "GHI");
        bulk->writeRow();        
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT i, ch FROM char4 order by i"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);    
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    ASSERT_EQ(mysql_num_rows(result), 3);
    MYSQL_ROW row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "1");
    ASSERT_STREQ(row[1], "ABC");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "2");
    ASSERT_STREQ(row[1], "DEF");
    row = mysql_fetch_row(result);
    ASSERT_STREQ(row[0], "3");
    ASSERT_STREQ(row[1], "GHI");
    mysql_free_result(result);
    
    if (mysql_query(my_con, "DROP TABLE char4"))
        FAIL() << "Could not drop table: " << mysql_error(my_con);
    delete bulk;
    delete driver;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
