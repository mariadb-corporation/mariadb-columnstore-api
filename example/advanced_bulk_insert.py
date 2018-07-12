#!/usr/bin/python3

# Copyright (c) 2018, MariaDB Corporation. All rights reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA 02110-1301  USA

## Advanced insert example from docs, requires the table in
## advanced_bulk_insert.sql to be created in the test database
##
## NOTE: if you edit this file please update the line numbers in
## advanced_bulk_insert.rst

import pymcsapi

try:
    driver = pymcsapi.ColumnStoreDriver()
    bulk = driver.createBulkInsert("test", "t2", 0, 0)
    bulk.setColumn(0, 1)
    bulk.setColumn(1, "Andrew")
    bulk.setColumn(2, "1936-12-24")
    bulk.setColumn(3, "2017-07-07 15:14:12")
    bulk.setColumn(4, "15239.45");
    bulk.writeRow()
    bulk.setColumn(0, 2)[0].setColumn(1, "David")[0].setColumn(2, pymcsapi.ColumnStoreDateTime(1972, 5, 23))[0].setColumn(3, pymcsapi.ColumnStoreDateTime(2017, 7, 7, 15, 20, 18))[0].setColumn(4, pymcsapi.ColumnStoreDecimal(2347623, 2))[0].writeRow()
    bulk.commit()
    summary = bulk.getSummary()
    print("Execution time: %s" % (summary.getExecutionTime(),))
    print("Rows inserted: %s" % (summary.getRowsInsertedCount(),))
    print("Truncation count: %s" %(summary.getTruncationCount(),))
    print("Saturated count: %s" %(summary.getSaturatedCount(),))
    print("Invalid count: %s" %(summary.getInvalidCount(),))
except RuntimeError as err:
    print("Error caught: %s" % (err,))
