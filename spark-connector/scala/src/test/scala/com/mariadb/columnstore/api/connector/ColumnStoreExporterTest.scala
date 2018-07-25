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

class ColumnStoreExporterTest {

  /**
    * Basic test to test the insertion of common data types.
    */
  @Test
  def testBasic() : Unit = {

    //initialize the spark session
    lazy val spark: SparkSession = SparkSession.builder.master("local").appName("spark-scala-connector-test").getOrCreate
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
    connectionProperties.put("driver", "org.mariadb.jdbc.Driver")
    if (sys.env.get("MCSAPI_CS_TEST_PASSWORD") != None){
        connectionProperties.put("password",sys.env.get("MCSAPI_CS_TEST_PASSWORD").get)
    }
    var connection: Connection = null
    try {
      connection = DriverManager.getConnection(url, connectionProperties)
      val statement = connection.createStatement
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest""")
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest2""")
      statement.executeQuery(
        """
        CREATE TABLE scalatest (
        uint64 bigint unsigned,
        int64 bigint,
        uint32 int unsigned,
        int32 int,
        uint16 smallint unsigned,
        int16 smallint,
        uint8 tinyint unsigned,
        `int8` tinyint,
        f float,
        d double,
        ch4 char(5),
        vch30 varchar(30),
        dt date,
        dtm datetime,
        dc decimal(18),
        tx text,
        bit tinyint(1),
        mathInt bigint unsigned,
        dc2 decimal(18,9))
        ENGINE=columnstore""")
        
    statement.executeQuery(
        """    
        CREATE TABLE scalatest2 (
        uint64 bigint unsigned,
        int64 bigint,
        uint32 int unsigned,
        int32 int,
        uint16 smallint unsigned,
        int16 smallint,
        uint8 tinyint unsigned,
        `int8` tinyint,
        f float,
        d double,
        ch4 char(5),
        vch30 varchar(30),
        dt date,
        dtm datetime,
        dc decimal(18),
        tx text,
        bit tinyint(1),
        mathInt bigint unsigned,
        dc2 decimal(18,9))
        ENGINE=columnstore""")

      //create the test dataframe
      val testDF = Seq(
        (1L, 2L, 3L, new Integer(4), new Integer(5), new Integer(6), new Integer(7), new Integer(8), 1.234F, 2.34567F, "ABCD", "Hello World", Date.valueOf("2017-09-08"), Timestamp.valueOf("2017-09-08 13:58:23"), new BigDecimal(123), "Hello World Longer", true, new BigInteger("9223372036854775807"), new BigDecimal("-0.000000001", MathContext.UNLIMITED)),
        (0L, -9223372036854775806L, 0L, new Integer(-2147483646), new Integer(0), new Integer(-32766), new Integer(0), new Integer(-126), 1.234F, 2.34567F, "A", "B", Date.valueOf("1000-01-01"), Timestamp.valueOf("1000-01-01 00:00:00"), new BigDecimal(-123), "C", false, new BigInteger("18446744073709551613"), new BigDecimal("100000000.999999999", MathContext.UNLIMITED)),
        (9223372036854775807L, 9223372036854775807L, 4294967293L, new Integer(2147483647), new Integer(65533), new Integer(32767), new Integer(253), new Integer(127), 1.234F, 2.34567F, "ZYXW", "012345678901234567890123456789", Date.valueOf("9999-12-31"), Timestamp.valueOf("9999-12-31 23:59:59"), new BigDecimal(123), "012345678901234567890123456789", true, new BigInteger("2342"), new BigDecimal("23.42")),
        (42L, 43L, 44L, null.asInstanceOf[Integer], null.asInstanceOf[Integer], null.asInstanceOf[Integer], null.asInstanceOf[Integer], null.asInstanceOf[Integer], 3.45F, 556.3F, null, null, null, null, null, null, false, null, null)
      ).toDF("uint64", "int64", "uint32", "int32", "uint16", "int16", "uint8", "int8", "f", "d", "ch4", "vch30", "dt", "dtm", "dc", "tx", "bit", "mathInt", "dc2")

      //write the test dataframe into columnstore
      ColumnStoreExporter.export("test", "scalatest", testDF)
      if (sys.env.get("COLUMNSTORE_INSTALL_DIR") != None){
        ColumnStoreExporter.export("test", "scalatest2", testDF, sys.env.get("COLUMNSTORE_INSTALL_DIR").get+"/etc/Columnstore.xml")
      }else{
        ColumnStoreExporter.export("test", "scalatest2", testDF, "/usr/local/mariadb/columnstore/etc/Columnstore.xml")
      }
      
      verifyAllTypes(connection, 1L, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.345669984817505, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23.0, 123, Hello World Longer, true, 9223372036854775807, -1E-9")
      verifyAllTypes(connection, 0L, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.345669984817505, A, B, 1000-01-01, 1000-01-01 00:00:00.0, -123, C, false, 18446744073709551613, 100000000.999999999")
      verifyAllTypes(connection, 9223372036854775807L, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.345669984817505, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59.0, 123, 012345678901234567890123456789, true, 2342, 23.420000000")
      verifyAllTypes(connection, 42L, "42, 43, 44, null, null, null, null, null, 3.45, 556.2999877929688, null, null, null, null, null, null, false, null, null")
      verifyAllTypes2(connection, 1L, "1, 2, 3, 4, 5, 6, 7, 8, 1.234, 2.345669984817505, ABCD, Hello World, 2017-09-08, 2017-09-08 13:58:23.0, 123, Hello World Longer, true, 9223372036854775807, -1E-9")
      verifyAllTypes2(connection, 0L, "0, -9223372036854775806, 0, -2147483646, 0, -32766, 0, -126, 1.234, 2.345669984817505, A, B, 1000-01-01, 1000-01-01 00:00:00.0, -123, C, false, 18446744073709551613, 100000000.999999999")
      verifyAllTypes2(connection, 9223372036854775807L, "9223372036854775807, 9223372036854775807, 4294967293, 2147483647, 65533, 32767, 253, 127, 1.234, 2.345669984817505, ZYXW, 012345678901234567890123456789, 9999-12-31, 9999-12-31 23:59:59.0, 123, 012345678901234567890123456789, true, 2342, 23.420000000")
      verifyAllTypes(connection, 42L, "42, 43, 44, null, null, null, null, null, 3.45, 556.2999877929688, null, null, null, null, null, null, false, null, null")
 
      //drop the test table
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest""")
      statement.executeQuery("""DROP TABLE IF EXISTS scalatest2""")
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
    val QUERY_ALL_TYPES = "select uint64, int64, uint32, int32, uint16, int16, uint8, `int8`, " +
      "f, d, ch4, vch30, dt, dtm, dc, tx, bit, mathInt, dc2 from scalatest where uint64 = ?"
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
    val QUERY_ALL_TYPES = "select uint64, int64, uint32, int32, uint16, int16, uint8, `int8`, " +
      "f, d, ch4, vch30, dt, dtm, dc, tx, bit, mathInt, dc2 from scalatest2 where uint64 = ?"
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


