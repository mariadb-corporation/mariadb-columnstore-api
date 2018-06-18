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
    my_con = mysql_init(NULL);
    if (!my_con)
        FAIL() << "Could not init MariaDB connection";
    if (!mysql_real_connect(my_con, "127.0.0.1", "root", "", NULL, 3306, NULL, 0))
        FAIL() << "Could not connect to MariaDB: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE DATABASE IF NOT EXISTS mcsapi"))
        FAIL() << "Error creating database: " << mysql_error(my_con);
    if (mysql_select_db(my_con, "mcsapi"))
        FAIL() << "Could not select DB: " << mysql_error(my_con);
    if (mysql_query(my_con, "DROP TABLE IF EXISTS mcol1408"))
        FAIL() << "Could not drop existing table: " << mysql_error(my_con);
    if (mysql_query(my_con, "CREATE TABLE IF NOT EXISTS mcol1408 (a varchar(20), b int, c char(5), d datetime) engine=columnstore"))
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


/* Test that fbo extents succeed */
TEST(mcol1408, mcol1408)
{
    const char* device_names[] = {"SBS01", "SBS02", "SBS03", "SBS04", "SBS05"};
    std::string table("mcol1408");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    srand(time(NULL));
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        // Aim for boundary of fbo > 0
        for (int i = 0; i < 33552384; i++)
        {
            double rnd = (double) rand() / (double) RAND_MAX;
            int rnum = rand();
            if (rnd < 0.2)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 60);
                bulk->setColumn(0, "Flow");
            }
            else if (rnd < 0.55)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 20)) + 15);
                bulk->setColumn(0, "Temperature");
            }
            else if (rnd < 0.7)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 50);
                bulk->setColumn(0, "Humidity");
            }
            else
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 100);
                bulk->setColumn(0, "Sound");
            }

            int dev_id = rand() % 5;
            bulk->setColumn(2, device_names[dev_id]);
            mcsapi::ColumnStoreDateTime tData;
            time_t t = time(0);
            tm *now = localtime(&t);
            tData.set(*now);
            bulk->setColumn(3, tData);
	        bulk->writeRow();
        }
        bulk->commit();
        delete bulk;
        delete driver;
        // This one will fail with MCOL-1408
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        bulk->setColumn(0, "EndTest");
        bulk->setColumn(1, 100);
        bulk->setColumn(2, "TEST1");
        bulk->setColumn(3, "0000-00-00 00:00:00");
        bulk->writeRow();
        bulk->commit();
        delete bulk;
        delete driver;
    } catch (mcsapi::ColumnStoreError &e) {
        if (bulk) bulk->rollback();
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    if (mysql_query(my_con, "SELECT COUNT(*) FROM mcol1408"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
	mysql_free_result(result);
}

TEST(mcol1408, BRM)
{
    const char* device_names[] = {"SBS01", "SBS02", "SBS03", "SBS04", "SBS05"};
    std::string table("mcol1408");
    std::string db("mcsapi");
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    srand(time(NULL));
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert(db, table, 0, 0);
        // Going over boundary in single commit causes BRM error
        for (int i = 0; i < 33554432; i++)
        {
            double rnd = (double) rand() / (double) RAND_MAX;
            int rnum = rand();
            if (rnd < 0.2)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 60);
                bulk->setColumn(0, "Flow");
            }
            else if (rnd < 0.55)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 20)) + 15);
                bulk->setColumn(0, "Temperature");
            }
            else if (rnd < 0.7)
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 50);
                bulk->setColumn(0, "Humidity");
            }
            else
            {
                bulk->setColumn(1, (rnum / (RAND_MAX / 40)) + 100);
                bulk->setColumn(0, "Sound");
            }

            int dev_id = rand() % 5;
            bulk->setColumn(2, device_names[dev_id]);
            mcsapi::ColumnStoreDateTime tData;
            time_t t = time(0);
            tm *now = localtime(&t);
            tData.set(*now);
            bulk->setColumn(3, tData);
	        bulk->writeRow();
        }
        bulk->commit();
    } catch (mcsapi::ColumnStoreError &e) {
        if (bulk) bulk->rollback();
        FAIL() << "Error caught: " << e.what() << std::endl;
    }
    delete bulk;
    delete driver;
    if (mysql_query(my_con, "SELECT COUNT(*) FROM mcol1408"))
        FAIL() << "Could not run test query: " << mysql_error(my_con);
    MYSQL_RES* result = mysql_store_result(my_con);
    if (!result)
        FAIL() << "Could not get result data: " << mysql_error(my_con);
	mysql_free_result(result);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new TestEnvironment);
  return RUN_ALL_TESTS();
}
