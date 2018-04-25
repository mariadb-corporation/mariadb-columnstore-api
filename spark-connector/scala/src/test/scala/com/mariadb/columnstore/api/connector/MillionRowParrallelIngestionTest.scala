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

import java.util.Properties
import org.apache.spark.sql.{SparkSession,DataFrame}
import org.apache.spark.sql.functions.{rand, randn}
import org.junit.Test
import org.junit.Assert._
import java.sql.{DriverManager,Connection,ResultSet,SQLException}

class MillionRowParrallelIngestionTest {
  val PARTITIONS = 25
  val ROWS = 5000000

  /**
    * Parallel ingestion test of 1Million rows of 25 partitions writing simultaneously to CS
    */
  @Test
  def testParallelMillionRow() : Unit = {

    //initialize the spark session
    lazy val spark: SparkSession = SparkSession.builder.master("local").appName("spark-scala-connector-1Million-row-test").getOrCreate
    import spark.implicits._
    val sc = spark.sparkContext
    val sqlContext = spark.sqlContext

    //create the test table
    val url = "jdbc:mysql://127.0.0.1:3306/test"
    val connectionProperties = new Properties()
    connectionProperties.put("user", "root")
    connectionProperties.put("driver", "org.mariadb.jdbc.Driver")
    var connection: Connection = null
    try {
      connection = DriverManager.getConnection(url, connectionProperties)
      val statement = connection.createStatement
      statement.executeQuery("""DROP TABLE IF EXISTS scalaSparkMillionRowTest""")
      statement.executeQuery("CREATE TABLE IF NOT EXISTS scalaSparkMillionRowTest (id BIGINT, uniform DOUBLE, normal DOUBLE) engine=columnstore")

      //create the test dataframe
      val testDF = sqlContext.range(0, ROWS).withColumn("uniform", rand(seed=23)).withColumn("normal", randn(seed=42)).repartition(PARTITIONS)

      //write the test dataframe into columnstore
      ColumnStoreExporter.export("test", "scalaSparkMillionRowTest", testDF)
      
      //verify the number of ingestions
      verifyIngestion(connection)
 
      //drop the test table
      statement.executeQuery("""DROP TABLE IF EXISTS scalaSparkMillionRowTest""")
    } catch {
      case e: Exception => fail("error during test: " + e)
    } finally {
      connection.close()
    }
  }

  /**
    * Tests if all data was ingested successfully
    */
  private def verifyIngestion(conn: Connection) : Unit = {
	var rs: ResultSet = null
	var entries = -1
    try {
      val statement = conn.createStatement
      rs = statement.executeQuery("""SELECT COUNT(*) AS cnt FROM scalaSparkMillionRowTest""")
      assertTrue(rs.next)
      entries = rs.getInt("cnt")
      assertEquals(ROWS, entries)
    }
    catch {
      case e: SQLException => fail("Error while validating the ingestion. Only " + entries + " of " + ROWS + " were ingested.")
    }
    finally {
      rs.close()
    }
  }
}
