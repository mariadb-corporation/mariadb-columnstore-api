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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS syscat"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS syscat (a int, b int) engine=columnstore"))
        FAIL() << "Could not create table: " << mysql_error(my_con);
  }
  // Override this to define how to tear down the environment.
  virtual void TearDown()
  {
    if (my_con)
    {
        if (mysql_query(my_con, "DROP TABLE syscat"))
            FAIL() << "Could not drop table: " << mysql_error(my_con);
        mysql_close(my_con);
    }
  }
};


/* Test that syscat works */
TEST(SystemCatalog, NormalUsage)
{
    std::string table("syscat");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        mcsapi::ColumnStoreSystemCatalog sysCat = driver->getSystemCatalog();
        mcsapi::ColumnStoreSystemCatalogTable tbl = sysCat.getTable(db, table);
        ASSERT_STREQ(tbl.getTableName().c_str(), table.c_str());
        ASSERT_STREQ(tbl.getSchemaName().c_str(), db.c_str());
        // Minimum OID for a public table is 3000
        ASSERT_GE(tbl.getOID(), 3000);
        mcsapi::ColumnStoreSystemCatalogColumn col1 = tbl.getColumn(0);
        ASSERT_GE(col1.getOID(), 3000);
        ASSERT_STREQ(col1.getColumnName().c_str(), "a");
        ASSERT_EQ(0, col1.getDictionaryOID());
        ASSERT_EQ(mcsapi::DATA_TYPE_INT, col1.getType());
        ASSERT_EQ(4, col1.getWidth());
        ASSERT_EQ(0, col1.getPosition());
        ASSERT_STREQ("", col1.getDefaultValue().c_str());
        ASSERT_EQ(0, col1.isAutoincrement());
        ASSERT_EQ(10, col1.getPrecision());
        ASSERT_EQ(0, col1.getScale());
        ASSERT_EQ(true, col1.isNullable());
        ASSERT_EQ(2, col1.compressionType());

        mcsapi::ColumnStoreSystemCatalogColumn col2 = tbl.getColumn("b");
        ASSERT_GE(col2.getOID(), 3000);
        ASSERT_STREQ(col2.getColumnName().c_str(), "b");
        ASSERT_EQ(0, col2.getDictionaryOID());
        ASSERT_EQ(mcsapi::DATA_TYPE_INT, col2.getType());
        ASSERT_EQ(4, col2.getWidth());
        ASSERT_EQ(1, col2.getPosition());
        ASSERT_STREQ("", col2.getDefaultValue().c_str());
        ASSERT_EQ(0, col2.isAutoincrement());
        ASSERT_EQ(10, col2.getPrecision());
        ASSERT_EQ(0, col2.getScale());
        ASSERT_EQ(true, col2.isNullable());
        ASSERT_EQ(2, col2.compressionType());

    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete driver;
}

/* Test that syscat table fails */
TEST(SystemCatalog, BadTable)
{
    std::string table("☃☃☃☃☃☃");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        mcsapi::ColumnStoreSystemCatalog sysCat = driver->getSystemCatalog();
        ASSERT_THROW(sysCat.getTable(db, table), mcsapi::ColumnStoreNotFound);
    }  catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete driver;
}

/* Test that syscat column name fails */
TEST(SystemCatalog, BadColumnName)
{
    std::string table("syscat");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        mcsapi::ColumnStoreSystemCatalog sysCat = driver->getSystemCatalog();
        mcsapi::ColumnStoreSystemCatalogTable tbl = sysCat.getTable(db, table);
        ASSERT_THROW(tbl.getColumn("❄❄❄"), mcsapi::ColumnStoreNotFound);
    }  catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete driver;
}

/* Test that syscat column number fails */
TEST(SystemCatalog, BadColumnNumber)
{
    std::string table("syscat");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        mcsapi::ColumnStoreSystemCatalog sysCat = driver->getSystemCatalog();
        mcsapi::ColumnStoreSystemCatalogTable tbl = sysCat.getTable(db, table);
        ASSERT_THROW(tbl.getColumn(32), mcsapi::ColumnStoreNotFound);
    }  catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete driver;
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
