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

import java.math.{BigDecimal,BigInteger,MathContext}
import org.apache.spark.sql.SparkSession
import org.junit.Test
import java.util.Properties
import org.junit.Assert._
import java.sql.{DriverManager,Connection,Date,Timestamp,PreparedStatement,ResultSet,SQLException}

class ColumnStoreSQLGenerationTest {

  /*
   * ColumnStoreExporter.parseTableColumnNameToCSConvention test cases
  */
  
  /**
   * Test if valid column and table names are handled correctly
  */
  @Test
  def testValidTableColumnNameToCSConventionParsing() : Unit = {
    val validTableColumnNames = Array("hello", "testing_21", "i_like_columnstore", "thisIsFun", "aBcDe", "AbcdEF")
    for(tableColumnName <- validTableColumnNames){
        var parsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
        assertEquals(tableColumnName, parsed)
    }
  }

  /**
   * Test if reserved words are handled correctly
  */
  @Test
  def testReservedWordParsing() : Unit = {
      val reservedWords = Array("integer", "int", "COLUmN", "DATABASE", "DOUBLE", "MASTER_HEARTBEAT_PERIOD")
      for(tableColumnName <- reservedWords){
          var parsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
          assertNotEquals(tableColumnName, parsed)
          var doubleParsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(parsed)
          assertEquals(doubleParsed, parsed)
      }
  }

  /**
   * Test if invalid words are handled correctly
  */
  @Test
  def testInvalidWordParsing() : Unit = {
      val invalidWords = Array("öl", "roHöl", "_hello*&@PKJEk", "lksf3823DMO[]", "l3mo()ntree", "An@ther")
      for(tableColumnName <- invalidWords){
          var parsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
          assertNotEquals(tableColumnName, parsed)
          var doubleParsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(parsed)
          assertEquals(doubleParsed, parsed)
      }
  }

  /**
   * Test if too long words are handled correctly
  */
  @Test
  def testTooLongWordParsing() : Unit = {
      val tooLongWords = Array("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab","ööööööööööööööööööööööööööööööööööööööööööööööööööööööööööööööö")
      for(tableColumnName <- tooLongWords){
          var parsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(tableColumnName)
          assertNotEquals(tableColumnName, parsed)
          var doubleParsed = ColumnStoreExporter.parseTableColumnNameToCSConvention(parsed)
          assertEquals(doubleParsed, parsed)
      }
  }

  /*
  * generateTableStatement() test cases
  */
  
  /**
    * Test the insertion of common data types in tables created with generateTableStatement()
    */
  @Test
  def testSQLGeneration() : Unit = {

    //initialize the spark session
    lazy val spark: SparkSession = SparkSession.builder.master("local").appName("spark-scala-connector-sql-generation-test").getOrCreate
    import spark.implicits._

    //get connection data
    var host = "127.0.0.1"
    var user = "root"
    if (sys.env.get("MCSAPI_CS_TEST_IP") != None){
        host = sys.env.get("MCSAPI_CS_TEST_IP").get
    }
    if (sys.env.get("MCSAPI_CS_TEST_USER") != None){
        user = sys.env.get("MCSAPI_CS_TEST_USER").get
    }
    
    //create the test table
    val url = "jdbc:mysql://"+host+":3306/test"
    val connectionProperties = new Properties()
    connectionProperties.put("user", user)
    if (sys.env.get("MCSAPI_CS_TEST_PASSWORD") != None){
        connectionProperties.put("password",sys.env.get("MCSAPI_CS_TEST_PASSWORD").get)
    }
    connectionProperties.put("driver", "org.mariadb.jdbc.Driver")
    var connection: Connection = null
    try {
      connection = DriverManager.getConnection(url, connectionProperties)
      val statement = connection.createStatement
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest_gen_1""")
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest_gen_2""")

      //create the test dataframe
      val testDF = Seq(
        (1L, 2L, 3L, 4, 5, 6, 7, 8.toShort, 1.234F, 2.34567.toDouble, "ABCD", "Hello World", Date.valueOf("2017-09-08"), Timestamp.valueOf("2017-09-08 13:58:23"), new BigDecimal(123), "Hello World Longer", true, new BigInteger("9223372036854775807"), new BigDecimal("-0.000000001", MathContext.UNLIMITED)),
        (0L, -9223372036854775806L, 0L, -2147483646, 0, -32766, 0, -126.toShort, 1.234F, 2.34567.toDouble, "A", "B", Date.valueOf("1000-01-01"), Timestamp.valueOf("1000-01-01 00:00:00"), new BigDecimal(-123), "C", false, new BigInteger("18446744073709551613"), new BigDecimal("100000000.999999999", MathContext.UNLIMITED)),
        (9223372036854775807L, 9223372036854775807L, 4294967293L, 2147483647, 65533, 32767, 253, 127.toShort, 1.234F, 2.34567.toDouble, "ZYXW", "012345678901234567890123456789", Date.valueOf("9999-12-31"), Timestamp.valueOf("9999-12-31 23:59:59"), new BigDecimal(123), "012345678901234567890123456789", true, new BigInteger("2342"), new BigDecimal("23.42"))
      ).toDF("uint64", "int64", "uint32", "int32", "uint16", "int16", "uint8", "int8", "f", "d", "ch4", "vch30", "dt", "dtm", "dc", "tx", "bit", "mathInt", "dc2")

      //create the tables using generateTableStatement()
      val statement1 = ColumnStoreExporter.generateTableStatement(testDF, "test", "scalatest_gen_1", true)
      println("executing create table statement: ")
      println(statement1)
      statement.executeQuery(statement1)
      val statement2 = ColumnStoreExporter.generateTableStatement(testDF, "test", "scalatest_gen_2")
      println("executing create table statement: ")
      println(statement2)
      statement.executeQuery(statement2)
      
      //write the test dataframe into columnstore
      ColumnStoreExporter.export("test", "scalatest_gen_1", testDF)
      ColumnStoreExporter.export("test", "scalatest_gen_2", testDF)
      
      verifyAllTypes(connection, 1L, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23.0, 123, Hello World Longer, true, 9223372036854775807, -1E-9")
      verifyAllTypes(connection, 0L, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00.0, -123, C, false, 18446744073709551613, 100000000.999999999")
      verifyAllTypes(connection, 9223372036854775807L, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59.0, 123, 012345678901234567890123456789, true, 2342, 23.420000000")
      verifyAllTypes2(connection, 1L, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.34567, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23.0, 123.000000000, Hello World Longer, true, 999999999999999999, -1E-9")
      verifyAllTypes2(connection, 0L, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.34567, A, B, 1000-01-01, 1000-01-01 00:00:00.0, -123.000000000, C, false, 999999999999999999, 100000000.999999999")
      verifyAllTypes2(connection, 9223372036854775807L, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.34567, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59.0, 123.000000000, 012345678901234567890123456789, true, 2342, 23.420000000")
      
      //drop the test table
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest_gen_1""")
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest_gen_2""")
    } catch {
      case e: Exception => fail("error during test: " + e)
    } finally {
      connection.close()
    }
  }

  /**
    * Tests if the data stored in the database equals the expected value.
    * @param conn, MariaDB connection
    * @param id, id of the database row
    * @param expected, the expected value
    */
  private def verifyAllTypes(conn: Connection, id: Long, expected: String) : Unit = {
    val QUERY_ALL_TYPES = "select uint64, int64, uint32, int32, uint16, int16, uint8, int8_rw, " +
      "f, d, ch4, vch30, dt, dtm, dc, tx, bit_rw, mathInt, dc2 from scalatest_gen_1 where uint64 = ?"
    var stmt: PreparedStatement = null
    var rs: ResultSet = null
    try {
      stmt = conn.prepareStatement(QUERY_ALL_TYPES)
      stmt.setLong(1, id)
      rs = stmt.executeQuery
      assertTrue(rs.next)
      val str = new StringBuffer
      val colCount = stmt.getMetaData.getColumnCount
      for (i <- 1 to colCount){
        if (i>1) str.append(", ")
        str.append(rs.getObject(i))
      }
      assertEquals(expected, str.toString)
    }
    catch {
      case e: SQLException => fail("Error while validating all_types results for id: " + id + ", error:" + e)
    }
    finally {
      rs.close()
      stmt.close()
    }
  }
  
    /**
    * Tests if the data stored in the database equals the expected value.
    * @param conn, MariaDB connection
    * @param id, id of the database row
    * @param expected, the expected value
    */
  private def verifyAllTypes2(conn: Connection, id: Long, expected: String) : Unit = {
    val QUERY_ALL_TYPES = "select uint64, int64, uint32, int32, uint16, int16, uint8, int8_rw, " +
      "f, d, ch4, vch30, dt, dtm, dc, tx, bit_rw, mathInt, dc2 from scalatest_gen_2 where uint64 = ?"
    var stmt: PreparedStatement = null
    var rs: ResultSet = null
    try {
      stmt = conn.prepareStatement(QUERY_ALL_TYPES)
      stmt.setLong(1, id)
      rs = stmt.executeQuery
      assertTrue(rs.next)
      val str = new StringBuffer
      val colCount = stmt.getMetaData.getColumnCount
      for (i <- 1 to colCount){
        if (i>1) str.append(", ")
        str.append(rs.getObject(i))
      }
      assertEquals(expected, str.toString)
    }
    catch {
      case e: SQLException => fail("Error while validating all_types results for id: " + id + ", error:" + e)
    }
    finally {
      rs.close()
      stmt.close()
    }
  }
}

