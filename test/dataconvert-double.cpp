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
    if (mysql_query(my_con, "DROP TABLE IF EXISTS dataconvertdouble"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS dataconvertdouble (a int, b double, c varchar(50)) engine=columnstore"))
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


/* Test that data convert from double works */
TEST(DataConvertDouble, DataConvertDouble)
{
    std::string table("dataconvertdouble");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk->setColumn(0, (double) 1);
        bulk->setColumn(1, (double) 1);
        bulk->setColumn(2, (double) 1);
        bulk->writeRow();
        bulk->setColumn(0, (double) 2147483647);
        bulk->setColumn(1, (double) 2147483647);
        bulk->setColumn(2, (double) 2147483647);
        bulk->writeRow();
        bulk->setColumn(0, (double) 3.14159);
        bulk->setColumn(1, (double) 3.14159);
        bulk->setColumn(2, (double) 3.14159);
        bulk->writeRow();
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT * FROM dataconvertdouble"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
    ASSERT_EQ(mysql_num_rows(result), 3);
    MYSQL_ROW row = mysql_fetch_row(result);
    std::string strval;
    double dblval;
    strval = row[0];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 1);
    strval = row[1];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 1);
    strval = row[2];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 1);
    row = mysql_fetch_row(result);
    strval = row[0];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 2147483647);
    strval = row[1];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 2147483647);
    strval = row[2];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 2147483647);
    row = mysql_fetch_row(result);
    strval = row[0];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, 3);
    strval = row[1];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, (double)3.14159);
    strval = row[2];
    dblval = std::stod(strval);
    ASSERT_DOUBLE_EQ(dblval, (double)3.14159);
    mysql_free_result(result);
    if (mysql_query(my_con, "DROP TABLE dataconvertdouble"))
        FAIL() << "Could not drop table: " << mysql_error(my_con);
    delete bulk;
    delete driver;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
