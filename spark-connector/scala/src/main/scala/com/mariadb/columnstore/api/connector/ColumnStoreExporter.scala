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

import com.mariadb.columnstore.api.{ColumnStoreDriver,ColumnStoreDecimal,columnstore_data_types_t}
import org.apache.spark.sql.DataFrame
import java.math.BigInteger

object ColumnStoreExporter {
  def export( database: String, table: String, df: DataFrame ) : Unit = {
    val rows = df.collect()
    val driver = new ColumnStoreDriver()
    val bulkInsert = driver.createBulkInsert(database, table, 0, 0)

    // get the column count of table
    val dbCatalog = driver.getSystemCatalog
    val dbTable = dbCatalog.getTable(database, table)
    val dbTableColumnCount = dbTable.getColumnCount

    // insert row by row into table
    try {
      for (row <- rows){
        for (columnId <- 0 until row.size){
          if (columnId < dbTableColumnCount){
            row.get(columnId) match {
              case input:Boolean => if (input) bulkInsert.setColumn(columnId, 1)
              else bulkInsert.setColumn(columnId, 0);
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
        bulkInsert.writeRow()
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
}
