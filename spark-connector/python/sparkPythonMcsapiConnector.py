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

def exportRddToColumnstore(database, table, df):
    
    global long

    if sys.version_info[0] == 3:
        long = int

    rows = df.collect()
    driver = pymcsapi.ColumnStoreDriver()
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
                        #temporary fix for MCOL-1133
                        dbColumn = dbTable.getColumn(columnId)
                        #DATA_TYPE_DECIMAL, DATA_TYPE_UDECIMAL
                        if dbColumn.getType() == 4 or dbColumn.getType() == 18:
                            negative = False
                            scale = 0
                            unscaledValue = int(row[columnId])
                            dcm = '{0:f}'.format(row[columnId]).split('.')
                            if len(dcm) == 2:
                                if dcm[0][0] == "-":
                                    negative = True
                                for i in dcm[1].rstrip("0"):
                                    scale += 1
                                    unscaledValue *= 10
                                    unscaledValue += int(i)
                                if negative and unscaledValue > 0:
                                    unscaledValue *= -1
                            bulkInsert.setColumn(columnId, pymcsapi.ColumnStoreDecimal(unscaledValue, scale))
                            
                        #DATA_TYPE_FLOAT, DATA_TYPE_FLOAT
                        elif dbColumn.getType() == 7 or dbColumn.getType() == 21:
                            bulkInsert.setColumn(columnId, float(row[columnId]))
                        #DATA_TYPE_DOUBLE, DATA_TYPE_UDOUBLE
                        elif dbColumn.getType() == 10 or dbColumn.getType() == 23:
                            bulkInsert.setColumn(columnId, float(row[columnId]))
                        #ANY OTHER DATA TYPE
                        else:
                            bulkInsert.setColumn(columnId, long(row[columnId]))
    
                        #solution once MCOL-1133 is fixed
                        #s = '{0:f}'.format(row[columnId])
                        #bulkInsert.setColumn(columnId, pymcsapi.ColumnStoreDecimal(s)))

                    #handle python2 unicode strings
                    elif sys.version_info[0] == 2 and isinstance(row[columnId], unicode):
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
