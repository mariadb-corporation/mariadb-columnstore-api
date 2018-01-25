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
import org.apache.spark.sql.functions.{rand, randn, sha1, sha2, md5}
import java.sql.{DriverManager,Connection}

object Benchmark {

  val url = "jdbc:mysql://localhost:3306"
  var connectionProperties = new Properties()
  lazy val spark: SparkSession = SparkSession.builder.master("local").appName("spark-scala-benchmark").getOrCreate
  import spark.implicits._
  val sc = spark.sparkContext
  val sqlContext = spark.sqlContext

  /**
    * Main function executing the benchmarks
    */
  def main(args: Array[String]): Unit = {
    sc.setLogLevel("WARN")
    connectionProperties.put("user", "root")
    connectionProperties.put("driver", "org.mariadb.jdbc.Driver")
    benchmark1
    println
    benchmark2
    emptyDatabase
  }
  
  private def createColumnStoreAPITable(name: String, schema: String) : Unit = {
    var connection: Connection = null
    try {
      connection = DriverManager.getConnection("jdbc:mysql://localhost:3306/benchmark", connectionProperties)
      val statement = connection.createStatement
      statement.executeQuery("CREATE TABLE IF NOT EXISTS " + name + " (" + schema + ") engine=columnstore")
    } catch {
      case e: Exception => e.printStackTrace()
    } finally {
      connection.close()
    }
  }
  
  private def emptyDatabase() : Unit = {
    var connection: Connection = null
    try {
      connection = DriverManager.getConnection(url, connectionProperties)
      val statement = connection.createStatement
      statement.executeQuery("""DROP DATABASE IF EXISTS benchmark""")
      statement.executeQuery("""CREATE DATABASE IF NOT EXISTS benchmark""")
    } catch {
      case e: Exception => e.printStackTrace()
    } finally {
      connection.close()
    }
  }

  /**
  * First benchmark comparing JDBC writes to InnoDB, JDBC writes to ColumnStore, and API writes to ColumnStore
  */
  private def benchmark1(): Unit ={
    println("===Benchmark 1===")
    println("Comparing JDBC writes to InnoDB, JDBC writes to ColumnStore and API writes to ColumnStore")
    println()
    
    emptyDatabase()

    println("creating dataframe 1: integers and their representation in ascii notation")
    val asciiDF = sc.makeRDD(0 until 128).map(i => (i.toChar.toString, i)).toDF("ascii_representation", "number").cache()
    val asciiDFRows = asciiDF.count()
    val asciiDFItems = asciiDFRows*asciiDF.columns.size
    asciiDF.printSchema()
    println("bemchmarking dataframe 1")
    val ascii_benchmark = benchmark1execution("ascii", asciiDF, "ascii_representation CHAR(1), number INT")
    asciiDF.unpersist()
    
    println("creating dataframe 2: two random generated doubles")
    val randDF = sqlContext.range(0, 1000).withColumn("uniform", rand(seed=23)).withColumn("normal", randn(seed=42)).cache()
    val randDFRows = randDF.count()
    val randDFItems = randDFRows*randDF.columns.size
    randDF.printSchema()
    println("bemchmarking dataframe 2")
    val rand_benchmark = benchmark1execution("rand", randDF, "id BIGINT, uniform DOUBLE, normal DOUBLE")
    randDF.unpersist()
    
    println("creating dataframe 3: sha1, sha256, sha512 and md5 hashes of integers")
    val tmpDF = sc.makeRDD(0 until 1000).map(i => (i, i.toString)).toDF("number", "string")
    tmpDF.createOrReplaceTempView("tempDF")
    val hashDF = sqlContext.sql("SELECT number, sha1(string) AS sha1, sha2(string,256) AS sha256, sha2(string,512) AS sha512, md5(string) AS md5 FROM tempDF").cache()
    val hashDFRows = hashDF.count()
    val hashDFItems = hashDFRows*hashDF.columns.size
    hashDF.printSchema()
    println("bemchmarking dataframe 3")
    val hash_benchmark = benchmark1execution("hash", hashDF, "number BIGINT, sha1 VARCHAR(40), sha256 VARCHAR(64), sha512 VARCHAR(128), md5 VARCHAR(32)")
    hashDF.unpersist()

    println("jdbc_innodb\tjdbc_columnstore\tapi_columnstore\t\trows\t\titems")
    println(ascii_benchmark._1/1000000000.toDouble+"s\t"+ascii_benchmark._2/1000000000.toDouble+"s\t\t"+ascii_benchmark._3/1000000000.toDouble+"s\t\t"+asciiDFRows+"\t\t"+asciiDFItems)
    println(rand_benchmark._1/1000000000.toDouble+"s\t"+rand_benchmark._2/1000000000.toDouble+"s\t\t"+rand_benchmark._3/1000000000.toDouble+"s\t\t"+randDFRows+"\t\t"+randDFItems)
    println(hash_benchmark._1/1000000000.toDouble+"s\t"+hash_benchmark._2/1000000000.toDouble+"s\t\t"+hash_benchmark._3/1000000000.toDouble+"s\t\t"+hashDFRows+"\t\t"+hashDFItems)
  }

  private def benchmark1execution(name: String, dataframe: DataFrame, schema: String) = {
    var t = System.nanoTime()
    dataframe.write.option("createTableOptions", "ENGINE=innodb").
      option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_innodb_"+name, connectionProperties)
    val jdbc_innodb_time = System.nanoTime() - t
    t = System.nanoTime()
    dataframe.write.option("numPartitions", 1).option("createTableOptions", "ENGINE=columnstore").
      option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_columnstore_"+name, connectionProperties)
    val jdbc_columnstore_time = System.nanoTime() - t
    t = System.nanoTime()
    createColumnStoreAPITable("api_columnstore_"+name, schema)
    ColumnStoreExporter.export("benchmark", "api_columnstore_"+name, dataframe)
    val api_columnstore_time = System.nanoTime() - t
    (jdbc_innodb_time, jdbc_columnstore_time, api_columnstore_time)
  }
  
  /**
  * Second benchmark comparing JDBC writes to InnoDB and API writes to ColumnStore with larger datasets
  */
  private def benchmark2(): Unit ={
    println("===Benchmark 2===")
    println("Comparing JDBC writes to InnoDB and API writes to ColumnStore with larger datasets")
    println()
    
    emptyDatabase()
    
    println("creating dataframe 1: two random generated doubles")
    val randDF = sqlContext.range(0, 7000000).withColumn("uniform", rand(seed=23)).withColumn("normal", randn(seed=42)).cache()
    val randDFRows = randDF.count()
    val randDFItems = randDFRows*randDF.columns.size
    randDF.printSchema()
    println("bemchmarking dataframe 1")
    val rand_benchmark = benchmark2execution("rand", randDF, "id BIGINT, uniform DOUBLE, normal DOUBLE")
    randDF.unpersist()
    
    println("creating dataframe 2: sha1, sha256, sha512 and md5 hashes of integers")
    val tmpDF = sc.makeRDD(0 until 3000000).map(i => (i, i.toString)).toDF("number", "string")
    tmpDF.createOrReplaceTempView("tempDF")
    val hashDF = sqlContext.sql("SELECT number, sha1(string) AS sha1, sha2(string,256) AS sha256, sha2(string,512) AS sha512, md5(string) AS md5 FROM tempDF").cache()
    val hashDFRows = hashDF.count()
    val hashDFItems = hashDFRows*hashDF.columns.size
    hashDF.printSchema()
    println("bemchmarking dataframe 2")
    val hash_benchmark = benchmark2execution("hash", hashDF, "number BIGINT, sha1 VARCHAR(40), sha256 VARCHAR(64), sha512 VARCHAR(128), md5 VARCHAR(32)")
    hashDF.unpersist()
    
    println("jdbc_innodb\tapi_columnstore\t\trows\t\titems")
    println(rand_benchmark._1/1000000000.toDouble+"s\t"+rand_benchmark._2/1000000000.toDouble+"s\t\t"+randDF.count+"\t\t"+randDF.count*randDF.columns.size)
    println(hash_benchmark._1/1000000000.toDouble+"s\t"+hash_benchmark._2/1000000000.toDouble+"s\t\t"+hashDF.count+"\t\t"+hashDF.count*hashDF.columns.size)
  }
  
  private def benchmark2execution(name: String, dataframe: DataFrame, schema: String) = {
    var t = System.nanoTime()
    dataframe.write.option("createTableOptions", "ENGINE=innodb").
    option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_innodb_"+name, connectionProperties)
    val jdbc_innodb_time = System.nanoTime() - t
    t = System.nanoTime()
    createColumnStoreAPITable("api_columnstore_"+name, schema)
    ColumnStoreExporter.export("benchmark", "api_columnstore_"+name, dataframe)
    val api_columnstore_time = System.nanoTime() - t
    (jdbc_innodb_time, api_columnstore_time)
  }
}

