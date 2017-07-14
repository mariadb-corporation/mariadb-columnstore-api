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

// Advanced insert example from docs, requires the table in
// advanced_bulk_insert.cpp to be created in the test database
//
// NOTE: if you edit this file please update the line numbers in
// advanced_bulk_insert.rst

#include <libmcsapi/mcsapi.h>
#include <iostream>

int main(void)
{
    mcsapi::ColumnStoreDriver* driver;
    mcsapi::ColumnStoreBulkInsert* bulk;
    try {
        driver = new mcsapi::ColumnStoreDriver();
        bulk = driver->createBulkInsert("test", "t2", 0, 0);
       	bulk->setColumn(0, 1);
       	bulk->setColumn(1, "Andrew");
        bulk->setColumn(2, "1936-12-24");
        bulk->setColumn(3, "2017-07-07 15:14:12");
        bulk->setColumn(4, "15239.45");
       	bulk->writeRow();
        mcsapi::ColumnStoreDateTime dob;
        dob.set("1972-05-23", "%Y-%m-%d");
        mcsapi::ColumnStoreDateTime added;
        added.set("2017-07-07 15:20:18", "%Y-%m-%d %H:%M:%S");
        mcsapi::ColumnStoreDecimal salary;
        salary.set(2347623, 2);
        bulk->setColumn(0, 2)->setColumn(1, "David")->setColumn(2, dob)
            ->setColumn(3, added)->setColumn(4, salary)->writeRow();
        bulk->commit();
        mcsapi::ColumnStoreSummary summary = bulk->getSummary();
        std::cout << summary.getRowsInsertedCount() << " inserted in " <<
            summary.getExecutionTime() << " seconds" << std::endl;
    } catch (mcsapi::ColumnStoreException &e) {
        std::cout << "Error caught: " << e.what() << std::endl;
    }
    delete bulk;
    delete driver;
}
