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

import sys, pymcsapi, decimal, datetime
	
def export(database, table, df, configuration=None):
    global long
    python2 = True

    if sys.version_info[0] == 3:
        long = int
        python2 = False

    rows = df.collect()
    if configuration == None:
        driver = pymcsapi.ColumnStoreDriver()
    else:
        driver = pymcsapi.ColumnStoreDriver(configuration)
    bulkInsert = driver.createBulkInsert(database, table, 0, 0)
    
    # get the column count of table
    dbCatalog = driver.getSystemCatalog()
    dbTable = dbCatalog.getTable(database, table)
    dbTableColumnCount = dbTable.getColumnCount()
    
    # insert row by row into table
    try:
        for row in rows:
            for columnId in range(0, len(row)):
                if columnId < dbTableColumnCount:
                    if isinstance(row[columnId], bool):
                        if row[columnId]:
                            bulkInsert.setColumn(columnId, 1)
                        else:
                            bulkInsert.setColumn(columnId, 0)
                    
                    elif isinstance(row[columnId], datetime.date):
                        bulkInsert.setColumn(columnId, row[columnId].strftime('%Y-%m-%d %H:%M:%S'))
                    
                    elif isinstance(row[columnId], decimal.Decimal):
                        dbColumn = dbTable.getColumn(columnId)
                        #DATA_TYPE_DECIMAL, DATA_TYPE_UDECIMAL, DATA_TYPE_FLOAT, DATA_TYPE_UFLOAT, DATA_TYPE_DOUBLE, DATA_TYPE_UDOUBLE
                        if dbColumn.getType() == 4 or dbColumn.getType() == 18 or dbColumn.getType() == 7 or dbColumn.getType() == 21 or dbColumn.getType() == 10 or dbColumn.getType() == 23:
                            s = '{0:f}'.format(row[columnId])
                            bulkInsert.setColumn(columnId, pymcsapi.ColumnStoreDecimal(s))
                        #ANY OTHER DATA TYPE
                        else:
                            bulkInsert.setColumn(columnId, long(row[columnId]))
    
                    #handle python2 unicode strings
                    elif python2 and isinstance(row[columnId], unicode):
                        bulkInsert.setColumn(columnId, row[columnId].encode('utf-8'))

                    #any other datatype is inserted without parsing
                    else:
                        bulkInsert.setColumn(columnId, row[columnId])
            bulkInsert.writeRow()
        bulkInsert.commit()
    except Exception as e:
        bulkInsert.rollback()
        print(row[columnId], type(row[columnId]))
        print(type(e))
        print(e)
       
    #print a short summary of the insertion process
    summary = bulkInsert.getSummary()
    print("Execution time: %s" % (summary.getExecutionTime(),))
    print("Rows inserted: %s" % (summary.getRowsInsertedCount(),))
    print("Truncation count: %s" %(summary.getTruncationCount(),))
    print("Saturated count: %s" %(summary.getSaturatedCount(),))
    print("Invalid count: %s" %(summary.getInvalidCount(),))
