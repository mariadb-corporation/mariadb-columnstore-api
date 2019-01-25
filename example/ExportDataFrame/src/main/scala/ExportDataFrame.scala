/*
Copyright (c) 2019, MariaDB Corporation. All rights reserved.
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

import org.apache.spark.sql.{SparkSession,DataFrame}
import java.util.Properties
import java.sql.{DriverManager,Connection,PreparedStatement,SQLException}
import com.mariadb.columnstore.api.connector.ColumnStoreExporter

object DataFrameExportExample {
	def main(args: Array[String]) {
		// get a Spark session
		val spark = SparkSession.builder.appName("DataFrame export into MariaDB ColumnStore").getOrCreate()
		
		// generate a sample DataFrame to be exported
		import spark.implicits._
		val df = spark.sparkContext.makeRDD(0 to 127).map(i => (i, i.toChar.toString)).toDF("number", "ASCII_representation")
		
		// set the variables for a JDBC connection
		var host = "um1"
		var user = "root"
		var password = ""
		
		val url = "jdbc:mysql://"+host+":3306/"
		val connectionProperties = new Properties()
		connectionProperties.put("user", user)
		connectionProperties.put("password", password)
		connectionProperties.put("driver", "org.mariadb.jdbc.Driver")
		
		// generate the target table
		val createTableStatement = ColumnStoreExporter.generateTableStatement(df, "test", "spark_export")
		
		var connection: Connection = null
		try {
			connection = DriverManager.getConnection(url, connectionProperties)
			val statement = connection.createStatement
			statement.executeQuery("CREATE DATABASE IF NOT EXISTS test")
			statement.executeQuery(createTableStatement)
		}catch {
			case e: Exception => System.err.println("error during create table statement: " + e)
		} finally {
			connection.close()
		}
		
		// export the DataFrame
		ColumnStoreExporter.export("test", "spark_export", df)
		spark.stop()
	}
}