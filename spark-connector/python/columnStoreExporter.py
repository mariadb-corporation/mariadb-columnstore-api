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

#exports a dataframe to columnstore
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

#parses a table or column name to ColumnStore convention
def parseTableColumnNameToCSConvention(tableColumnName):
    if tableColumnName == None:
        return tableColumnName
    
    import re, os
    firstCharPattern = re.compile("[a-zA-Z0-9]")
    otherCharPattern = re.compile("[a-zA-Z0-9_]")
    maxTableColumnLength = 64
    prefix = "p_"
    postfix = "_rw"
    try:
        reservedWordsFile = os.path.join(os.path.dirname(__file__),"mcsapi_reserved_words.txt")
    except NameError:
        reservedWordsFile = "mcsapi_reserved_words.txt"

    out_str = []
    
    if firstCharPattern.match(tableColumnName[0:1]):
        out_str.append(tableColumnName[0:1])
    else:
        out_str.append(prefix)
        
    for i in range(1,len(tableColumnName)):
        if otherCharPattern.match(tableColumnName[i:i+1]):
            out_str.append(tableColumnName[i:i+1])
        else:
            out_str.append("_")
    
    #check if the length is below maxTableColumnLength
    tableColumnName = ''.join(out_str)
    if len(tableColumnName) > maxTableColumnLength:
        tableColumnName = tableColumnName[:maxTableColumnLength]
    
    #check if the name is a reserved word
    reservedWords = set()
    try:
        f = open(reservedWordsFile, "r")
        for l in f:
            reservedWords.add(l.lower().rstrip('\n'))
        f.close()
    except Exception as e:
        print("warning: wasn't able to load the reserved words file: %s" % (reservedWordsFile,))
        print(e)
    
    if tableColumnName.lower() in reservedWords:
        if (len(tableColumnName) + len(postfix)) > maxTableColumnLength:
            return tableColumnName[:maxTableColumnLength-len(postfix)] + postfix
        else:
            return tableColumnName + postfix
    else:
        return tableColumnName

#splits a decimal into its two parts and returns its lengths as well if it should support negatives
def splitDecimalGetLength(decimal):
    decimal_tuple = decimal.normalize().as_tuple()
    beforePoint = len(decimal_tuple.digits)+decimal_tuple.exponent
    if beforePoint < 0:
        beforePoint = 0
    afterPoint = decimal_tuple.exponent*-1
    negative = False
    if decimal_tuple.sign > 0:
        negative = True
    return (beforePoint, afterPoint, negative)

#generates a CREATE table statement based on the dataframes schema
def generateTableStatement(dataFrame, database=None, table="spark_export", determineTypeLengths=False):
    import pyspark.sql.types

    if determineTypeLengths:
        from pyspark.sql.functions import length as pySparkLength
        from pyspark.sql.functions import udf
    
    if database == None:
        prefix = "CREATE TABLE `" + parseTableColumnNameToCSConvention(table) + "` ("
    else:
        prefix = "CREATE TABLE `" + database + "`.`" + parseTableColumnNameToCSConvention(table) +"` ("
    postfix = ") ENGINE=columnstore;"
    
    column_list = []
    for column in dataFrame.schema:
        if isinstance(column, pyspark.sql.types.StructField):
            # data type mapping
            if isinstance(column.dataType, pyspark.sql.types.StringType):
                if determineTypeLengths:
                    #get the max String length from the column
                    length = dataFrame.select(pySparkLength(column.name).alias('str_length')).agg({"str_length": "max"}).head(1)[0][0]
                    if length <= 64:
                        columnType = "VARCHAR(" + str(length) + ")"
                    elif length <= 255:
                        columnType = "TINYTEXT"
                    elif length <= 65536:
                        columnType = "TEXT"
                    elif length <= 16777216:
                        columnType = "MEDIUMTEXT"
                    else:
                        columnType = "LONGTEXT"
                else:
                    columnType = "TINYTEXT"
            elif isinstance(column.dataType, pyspark.sql.types.BooleanType):
                columnType = "TINYINT"
            elif isinstance(column.dataType, pyspark.sql.types.DateType):
                columnType = "DATE"
            elif isinstance(column.dataType, pyspark.sql.types.TimestampType):
                columnType = "DATETIME"
            elif isinstance(column.dataType, pyspark.sql.types.DecimalType):
                if determineTypeLengths:
                    splitDecimalLengthType = pyspark.sql.types.StructType([
                        pyspark.sql.types.StructField("lengthBeforePoint", pyspark.sql.types.LongType(), False),
                        pyspark.sql.types.StructField("lengthAfterPoint", pyspark.sql.types.LongType(), False),
                        pyspark.sql.types.StructField("negative", pyspark.sql.types.BooleanType(), False)
                    ])

                    #get the maximal lengths for the column and check if it is negative
                    udf_splitDecimalGetLength = udf(splitDecimalGetLength, splitDecimalLengthType)
                    tmp = dataFrame.withColumn("splitDecimal", udf_splitDecimalGetLength(column.name))

                    maxLengthBeforePoint = tmp.agg({"splitDecimal.lengthBeforePoint": "max"}).head(1)[0][0]
                    maxLengthAfterPoint = tmp.agg({"splitDecimal.lengthAfterPoint": "max"}).head(1)[0][0]
                    
                    #set the precision and scale
                    if maxLengthBeforePoint + maxLengthAfterPoint <= 18:
                        columnType = "DECIMAL(" + str(maxLengthBeforePoint + maxLengthAfterPoint) + "," + str(maxLengthAfterPoint) + ")"
                    else:
                        if 18-maxLengthBeforePoint > 0:
                            maxLengthAfterPoints = 18-maxLengthBeforePoint
                            columnType = "DECIMAL(18," + str(maxLengthAfterPoints) + ")"
                        else:
                            negative = tmp.agg({"splitDecimal.negative": "max"}).head(1)[0][0]
                            if negative:
                                columnType = "BIGINT"
                            else:
                                columnType = "BIGINT UNSIGNED"
                else:
                    if column.dataType.scale == 0:
                        if column.dataType.precision <= 18:
                            columnType = "DECIMAL(" + str(column.dataType.precision) + "," + str(column.dataType.scale) + ")"
                        else:
                            columnType = "DECIMAL(18,0)"
                    elif column.dataType.precision <= 18 and column.dataType.scale <= 18 and column.dataType.scale <= column.dataType.precision:
                        columnType = "DECIMAL(" + str(column.dataType.precision) + "," + str(column.dataType.scale) + ")"
                    else:
                        columnType = "DECIMAL(18,9)"
            elif isinstance(column.dataType, pyspark.sql.types.DoubleType):
                columnType = "DOUBLE"
            elif isinstance(column.dataType, pyspark.sql.types.FloatType):
                columnType = "FLOAT"
            elif isinstance(column.dataType, pyspark.sql.types.IntegerType):
                columnType = "INTEGER"
            elif isinstance(column.dataType, pyspark.sql.types.LongType):
                columnType = "BIGINT"
            elif isinstance(column.dataType, pyspark.sql.types.ShortType):
                columnType = "SMALLINT"
            else:
                raise TypeError("unsupported datatype %s for column %s" % (column.dataType, column.name))
            # nullable or not
            if column.nullable:
                nullable = "NULL"
            else:
                nullable = "NOT NULL"
            column_list.append(parseTableColumnNameToCSConvention(column.name) + " " + columnType + " " + nullable + ", ")
        else:
            raise TypeError("unsupported datatype %s in dataframe" % (type(column),))
        
    return prefix + ''.join(column_list)[:-2] + postfix
