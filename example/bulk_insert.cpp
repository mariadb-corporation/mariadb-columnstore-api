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

#include <libcolumnstore1/mcsapi.h>
#include <iostream>

int main(void)
{
    std::string table("t1");
    std::string db("test");
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
        std::cout << "Error caught: " << e.what() << std::endl;
    }
    delete bulk;
    delete driver;
}