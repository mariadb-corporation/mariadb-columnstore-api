#!/bin/python

# Copyright (c) 2017, MariaDB Corporation. All rights reserved.
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

import sys, pymcsapi, csv

if len(sys.argv) != 4:
    print("required arguments: db-name table-name import-file")
    sys.exit(2)
    
driver = pymcsapi.ColumnStoreDriver()
catalog = driver.getSystemCatalog()
table = catalog.getTable(sys.argv[1], sys.argv[2])
columnCount = table.getColumnCount()
bulk = driver.createBulkInsert(sys.argv[1], sys.argv[2], 0, 0)
with open(sys.argv[3], 'rb') as csvfile:
    csvreader = csv.reader(csvfile, delimiter='|')
    for row in csvreader:
        for col in xrange(0, len(row)):
            if col < columnCount:
                bulk.setColumn(col, row[col])
        bulk.writeRow()
bulk.commit()

summary = bulk.getSummary()
print("Execution time: %s" % (summary.getExecutionTime(),))
print("Rows inserted: %s" % (summary.getRowsInsertedCount(),))
print("Truncation count: %s" %(summary.getTruncationCount(),))
print("Saturated count: %s" %(summary.getSaturatedCount(),))
print("Invalid count: %s" %(summary.getInvalidCount(),))
