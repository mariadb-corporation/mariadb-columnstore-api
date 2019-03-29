# Copyright (c) 2019, MariaDB Corporation. All rights reserved.
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

import columnStoreExporter
from pyspark.sql import SparkSession, Row
import mysql.connector as mariadb

#Create the spark session
spark = SparkSession.builder.appName("DataFrame export into MariaDB ColumnStore").getOrCreate()

#Generate the DataFrame to export
df = spark.createDataFrame(spark.sparkContext.parallelize(range(0, 128)).map(lambda i: Row(number=i, ASCII_representation= chr(i))))

#JDBC connection parameter
user = "root"
host = "um1"
password = ""

#Create the target table
createTableStatement = columnStoreExporter.generateTableStatement(df, "test", "pyspark_export")
try:
    conn = mariadb.connect(user=user, database='', host=host, password=password)
    cursor = conn.cursor()
    cursor.execute("CREATE DATABASE IF NOT EXISTS test")
    cursor.execute(createTableStatement)
    
except mariadb.Error as err:
    print("Error during table creation: ", err)
finally:
    if cursor: cursor.close()
    if conn: conn.close()

#Export the DataFrame into ColumnStore
columnStoreExporter.export("test","pyspark_export",df)
spark.stop()