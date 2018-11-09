package com.mariadb.columnstore.api.connector

/*
Copyright (c) 2018, MariaDB Corporation. All rights reserved.
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301  USA
*/

import com.mariadb.columnstore.api._
import org.apache.spark.sql.DataFrame
import org.apache.spark.sql.functions.{max,length,udf}
import org.apache.spark.sql.types
import java.math.BigInteger
import java.math.BigDecimal
import java.util.regex.Pattern
import java.io.{InputStream,BufferedReader,IOException,InputStreamReader}
import org.apache.spark.SparkContext
import org.apache.spark.rdd.RDD

object ColumnStoreExporter {

  /**
   * Parses an input String to CS naming conventions for table and column names
   * @param input, input String
   * @return parsed output String
  */
  def parseTableColumnNameToCSConvention(input: String) : String = {
    //set of reserved words that can't be used for table or column names
    var reservedWords: Set[String] = Set()
    val RESERVED_WORDS_FILENAME = "resources/mcsapi_reserved_words.txt"
    val CS_TABLE_COLUMN_NAMING_CONVENTION_PREFIX = "p_"
    val CS_TABLE_COLUMN_NAMING_CONVENTION_SUFFIX = "_rw"
    val MAX_TABLE_COLUMN_NAME_LENGTH = 64  
  
    //container for the output
    val output: StringBuilder = new StringBuilder()

    if(input == null){
      output.append("null")
    }else{
      //if the first character is lowercase [a-z] or uppercase [A-Z] use it
      if(Pattern.matches("[a-zA-Z]", input.substring(0,1))){
        output.append(input.substring(0,1))
      }else{ //otherwise add a prefix and discard the first character
        output.append(CS_TABLE_COLUMN_NAMING_CONVENTION_PREFIX)
      }

      //if the following characters match the allowed character set use them, otherwise use _
      for(e <- 2 to input.length()){
        if(Pattern.matches("[a-zA-Z0-9_]", input.substring(e-1,e))){
          output.append(input.substring(e-1,e))
        } else{
          output.append("_")
        }
      }
    }

    //check if the length is below maxTableColumnLength
    if(output.toString.length > MAX_TABLE_COLUMN_NAME_LENGTH){
        output.delete(MAX_TABLE_COLUMN_NAME_LENGTH,output.toString.length)
    }
    
    // Fill the set of reserved words from file reserved_words.txt
    if(getClass().getResource(RESERVED_WORDS_FILENAME) == null){
        println("warning: can't access the reserved words file: " + RESERVED_WORDS_FILENAME)
    }else{
        try{
            val is: InputStream = getClass().getResourceAsStream(RESERVED_WORDS_FILENAME)
            val reader: BufferedReader = new BufferedReader(new InputStreamReader(is))
            var line: String = null
            while ({line = reader.readLine; line != null}) {
              reservedWords += line.toLowerCase()
            }
            reader.close()
            is.close()
        } catch {
            case ex: IOException => println("error while processing the reserved words file: " + RESERVED_WORDS_FILENAME)
        }
    }
    
    //if the resulting output is a reserved word, add a suffix
    if(reservedWords.contains(output.toString().toLowerCase())){
      if((output.toString.length + CS_TABLE_COLUMN_NAMING_CONVENTION_SUFFIX.length) > MAX_TABLE_COLUMN_NAME_LENGTH){
          output.delete(MAX_TABLE_COLUMN_NAME_LENGTH-CS_TABLE_COLUMN_NAMING_CONVENTION_SUFFIX.length,output.toString.length)
      }
      output.append(CS_TABLE_COLUMN_NAMING_CONVENTION_SUFFIX)
    }
  
    return output.toString();
  }

  def splitDecimalGetLength(decimal :BigDecimal) : (Integer, Integer, Short) = {
      var lengthBeforePoint = 0
      var lengthAfterPoint = 0
      if (decimal.signum == -1){
          lengthBeforePoint = -1
      }
      val dec: String = decimal.stripTrailingZeros.toPlainString
      val split = dec.split("\\.")
      if (split.size > 1){
          lengthBeforePoint += split(0).size
          lengthAfterPoint = split(1).size
      } else{
          lengthBeforePoint += split(0).size
      }
      return (lengthBeforePoint: Integer, lengthAfterPoint: Integer, decimal.signum.toShort: Short)
  }

  def generateTableStatement(dataFrame: DataFrame, database: String = null, table: String = "spark_export", determineTypeLengths: Boolean = false) : String = {
      
      val output: StringBuilder = new StringBuilder("CREATE TABLE ")
      
      if (database != null){
          output.append(database).append(".")
      }
      output.append(parseTableColumnNameToCSConvention(table)).append(" (")
      
      //iterate through the dataFrame schema
      for (column <- dataFrame.schema){
          //check if the column is a struct
          if (column.isInstanceOf[org.apache.spark.sql.types.StructField]){
              output.append(parseTableColumnNameToCSConvention(column.name) + " ")
              column.dataType match {
                  case org.apache.spark.sql.types.BooleanType => output.append("TINYINT(1) ")
                  case org.apache.spark.sql.types.DateType => output.append("DATE ")
                  case input: org.apache.spark.sql.types.DecimalType =>
                  if (determineTypeLengths){
                      val splitDecimalLengthType = org.apache.spark.sql.types.StructType(
                          org.apache.spark.sql.types.StructField("lengthBeforePoint", org.apache.spark.sql.types.IntegerType, false) ::
                          org.apache.spark.sql.types.StructField("lengthAfterPoint", org.apache.spark.sql.types.IntegerType, false) ::
                          org.apache.spark.sql.types.StructField("signum", org.apache.spark.sql.types.ShortType, false) :: Nil
                      )
                      
                      //get the maximal lengths for the column and check if it is negative
                      val udf_splitDecimalGetLength = org.apache.spark.sql.functions.udf(splitDecimalGetLength(_: BigDecimal), splitDecimalLengthType)
                      val tmp = dataFrame.withColumn("splitDecimal", udf_splitDecimalGetLength(dataFrame.col(column.name)))
                      val max = tmp.agg(org.apache.spark.sql.functions.max("splitDecimal.lengthBeforePoint"), org.apache.spark.sql.functions.max("splitDecimal.lengthAfterPoint"), org.apache.spark.sql.functions.max("splitDecimal.signum")).head(1)
                      
                      val maxLengthBeforePoint: Integer = max(0)(0).asInstanceOf[Integer]
                      var maxLengthAfterPoint: Integer = max(0)(1).asInstanceOf[Integer]
                      val signum: Short = max(0)(2).asInstanceOf[Short]
  
                      if (maxLengthBeforePoint + maxLengthAfterPoint <= 18){
                          output.append("DECIMAL(" + (maxLengthBeforePoint + maxLengthAfterPoint) + "," + maxLengthAfterPoint + ") ")
                      }else {
                          if (18-maxLengthBeforePoint > 0){
                              maxLengthAfterPoint = 18-maxLengthBeforePoint
                              output.append("DECIMAL(18," + maxLengthAfterPoint + ") ")
                          }else {
                              if (signum == -1){
                                  output.append("BIGINT ")
                              }
                              else{
                                  output.append("BIGINT UNSIGNED ")
                              }
                          }
                      }
                  } else{
                      if(input.scale == 0){
                          if(input.precision <= 18){
                              output.append("DECIMAL (" + input.precision + "," + input.scale +") ")
                          }else{
                              output.append("DECIMAL (18,0) ")
                          }
                      }
                      else if (input.precision <= 18 && input.scale <= 18 && input.scale <= input.precision){
                          output.append("DECIMAL (" + input.precision + "," + input.scale +") ")
                      } else{
                          output.append("DECIMAL (18,9) ")
                      }
                  }
                  case org.apache.spark.sql.types.DoubleType => output.append("DOUBLE ")
                  case org.apache.spark.sql.types.FloatType => output.append("FLOAT ")
                  case org.apache.spark.sql.types.IntegerType => output.append("INTEGER ")
                  case org.apache.spark.sql.types.LongType => output.append("BIGINT ")
                  case org.apache.spark.sql.types.ShortType => output.append("SMALLINT ")
                  case org.apache.spark.sql.types.StringType => 
                  if (determineTypeLengths){
                      val lengthRow = dataFrame.agg(org.apache.spark.sql.functions.max(org.apache.spark.sql.functions.length(dataFrame.col(column.name)))).first
                      val length = lengthRow(0).asInstanceOf[Integer]
                      if (length <= 64){
                          output.append("VARCHAR(" + length + ") ")
                      }else if (length <= 255){
                          output.append("TINYTEXT ")
                      }else if (length <= 65536){
                          output.append("TEXT ")
                      }else if (length <= 16777216){
                          output.append("MEDIUMTEXT ")
                      }else{
                          output.append("LONGTEXT ")
                      }
                  } else{
                      output.append("TINYTEXT ")
                  }
                  case org.apache.spark.sql.types.TimestampType => output.append("DATETIME ")
                  case _ => throw new IllegalArgumentException("type " + column.dataType + " for column " + column.name + " in dataframe is currently not supported");
              }
              if (column.nullable){
                  output.append("NULL, ")
              }else{
                  output.append("NOT NULL, ")
              }
          } else{
              throw new IllegalArgumentException("type " + column.getClass + " in dataframe is currently not supported");
          }
      }
      
      output.delete(output.length-2,output.length).append(") ENGINE=COLUMNSTORE;")
      
      return output.toString();
  }

  /**
  / Writes a row into ColumnStore
  */
  @throws(classOf[Exception])
  def writeRow (row: org.apache.spark.sql.Row, dbTableColumnCount: Integer, dbTable: ColumnStoreSystemCatalogTable, bulkInsert: ColumnStoreBulkInsert) : Unit = {
  for (columnId <- 0 until row.size){
    if (columnId < dbTableColumnCount){
      if (row.get(columnId) == null){
        if(dbTable.getColumn(columnId).isNullable){
          bulkInsert.setNull(columnId)
        } else{
          System.err.println("warning: column " + columnId + " is not nullable. Using default value instead.")
          bulkInsert.setColumn(columnId, dbTable.getColumn(columnId).getDefaultValue())
        }
      } else{
          row.get(columnId) match {
            case input:Boolean => bulkInsert.setColumn(columnId, input)
            case input:Byte => bulkInsert.setColumn(columnId, input)
            case input:java.sql.Date => bulkInsert.setColumn(columnId, input.toString)
            case input:java.math.BigDecimal =>
              val dbColumn = dbTable.getColumn(columnId)
              if (dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_DECIMAL) ||
                  dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_UDECIMAL) ||
                  dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_FLOAT) ||
                  dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_UFLOAT) ||
                  dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_DOUBLE) ||
                  dbColumn.getType.equals(columnstore_data_types_t.DATA_TYPE_UDOUBLE)){
                    bulkInsert.setColumn(columnId, new ColumnStoreDecimal(input.toPlainString))
              }
              else {
                bulkInsert.setColumn(columnId, input.toBigInteger)
              }
            case input:Double => bulkInsert.setColumn(columnId, input)
            case input:Float => bulkInsert.setColumn(columnId, input)
            case input:Integer => bulkInsert.setColumn(columnId, input)
            case input:Long => bulkInsert.setColumn(columnId, input)
            case input:Short => bulkInsert.setColumn(columnId, input)
            case input:String => bulkInsert.setColumn(columnId, input)
            case input:java.sql.Timestamp => bulkInsert.setColumn(columnId, input.toString)
            case _ => throw new Exception("Parsing error, can't convert " +  row.get(columnId).getClass + ".")
          }
        }
      }
    }
    bulkInsert.writeRow()
  }
  
  /**
  * Export function to export a DataFrame into an existing ColumnStore table by writing everything in one transaction from the Spark Driver.
  */
  def export( database: String, table: String, df: DataFrame, configuration: String = "") : Unit = {
    var driver: ColumnStoreDriver = null
    if (configuration == ""){
      driver = new ColumnStoreDriver()
    }
    else{
      driver = new ColumnStoreDriver(configuration)
    }
    val bulkInsert = driver.createBulkInsert(database, table, 0, 0)

    // get the column count of table
    val dbCatalog = driver.getSystemCatalog
    val dbTable = dbCatalog.getTable(database, table)
    val dbTableColumnCount = dbTable.getColumnCount
    
    // insert row by row into table
    try {
      for (row <- df.rdd.toLocalIterator){
        writeRow(row, dbTableColumnCount, dbTable, bulkInsert)
      }
      bulkInsert.commit()
    }
    catch {
      case e: Exception => bulkInsert.rollback(); e.printStackTrace();
    }
    finally{ // print a short summary of the insertion process
      val summary = bulkInsert.getSummary
      println("Execution time: " + summary.getExecutionTime)
      println("Rows inserted: " + summary.getRowsInsertedCount)
      println("Truncation count: " + summary.getTruncationCount)
      println("Saturated count: " + summary.getSaturatedCount)
      println("Invalid count: " + summary.getInvalidCount)
    }
  }
  
  /**
  * Export function to export a RDD into an existing ColumnStore table by writing each partition as one transaction from the Spark Workers into ColumnStore.
  */
  def exportFromWorkers[Row] (database: String, table: String, rdd: RDD[Row], configuration: String = "") : Unit = {
    println("number of partitions: " + rdd.getNumPartitions)
    for(p <- 0 until rdd.getNumPartitions){
        println("Exporting partition " + p)
        rdd.sparkContext.runJob(rdd,(iter: Iterator[Row]) => {
             // initialize the ColumnStore Driver on the Worker 
            var driver: ColumnStoreDriver = null
            if (configuration == ""){
              driver = new ColumnStoreDriver()
            }
            else{
              driver = new ColumnStoreDriver(configuration)
            }
            val bulkInsert = driver.createBulkInsert(database, table, 0, 0)

            // get the column count of the CS table
            val dbCatalog = driver.getSystemCatalog
            val dbTable = dbCatalog.getTable(database, table)
            val dbTableColumnCount = dbTable.getColumnCount
            
            // insert row by row into table
            try{
              while(iter.hasNext){
                var row = iter.next()
                row match {
                    case row: org.apache.spark.sql.Row => writeRow(row, dbTableColumnCount, dbTable, bulkInsert)
                    case _ => println("Wasn't able to inject row: " + row + "\nThe row type doesn't match.")
                }
              }
              bulkInsert.commit()
            }
            catch {
              case e: Exception => bulkInsert.rollback(); e.printStackTrace();
            }
            finally{ // print a short summary of the insertion process
              val summary = bulkInsert.getSummary
              println("Execution time: " + summary.getExecutionTime)
              println("Rows inserted: " + summary.getRowsInsertedCount)
              println("Truncation count: " + summary.getTruncationCount)
              println("Saturated count: " + summary.getSaturatedCount)
              println("Invalid count: " + summary.getInvalidCount)
            }
        }, List(p))
    }
  }
}
