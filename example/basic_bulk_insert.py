#!/usr/bin/python

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

## Bulk insert example from docs, requires the table in
## basic_bulk_insert.sql to be created in the test database
##
## NOTE: if you edit this file please update the line numbers in
## basic_bulk_insert.rst

import pymcsapi

try:
    driver = pymcsapi.ColumnStoreDriver()
    bulk = driver.createBulkInsert("test", "t1", 0, 0)
    for i in range(0,1000):
        bulk.setColumn(0, i)
        bulk.setColumn(1, 1000-i)
        bulk.writeRow()
    bulk.commit()
except RuntimeError as err:
    print("Error caught: %s" % (err,))
