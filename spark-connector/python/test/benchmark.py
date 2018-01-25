#!/usr/bin/python

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

import columnStoreExporter, time, sys
import mysql.connector as mariadb
from pyspark import SparkContext
from pyspark.sql import Row, SQLContext
from pyspark.sql.functions import rand, randn, sha1, sha2, md5

def main():
    global long, python2, url, properties, sc, sqlContext
	
    python2 = True

    if sys.version_info[0] == 3:
        long = int
        python2 = False

    url = 'jdbc:mysql://localhost:3306'
    properties = {'user': 'root', 'driver': 'org.mariadb.jdbc.Driver'}
    sc = SparkContext("local", "MariaDB Spark ColumnStore Benchmark")
    sqlContext = SQLContext(sc)
    sc.setLogLevel("WARN")
    
    benchmark1()
    print("")
    benchmark2()
    emptyDatabase()
    

def emptyDatabase():
    try:
        conn = mariadb.connect(user='root', host='localhost')
        cursor = conn.cursor()
        cursor.execute("DROP DATABASE IF EXISTS benchmark")
        cursor.execute("CREATE DATABASE IF NOT EXISTS benchmark")

    except mariadb.Error as err:
        print("Error while preparing the database for the benchmark. %s" %(err,))

    finally:
        if cursor: cursor.close()
        if conn: conn.close()

        
def createColumnStoreAPITable(name, schema):
    try:
        conn = mariadb.connect(user='root', database='benchmark', host='localhost')
        cursor = conn.cursor()
        cursor.execute("CREATE TABLE IF NOT EXISTS %s (%s) engine=columnstore" %(name,schema))

    except mariadb.Error as err:
        print("Error while creating the columnstore database %s for the benchmark. %s" %(name,err,))
    
    finally:
        if cursor: cursor.close()
        if conn: conn.close()


def benchmark1():
    print("===Benchmark 1===")
    print("Comparing JDBC writes to InnoDB, JDBC writes to ColumnStore and API writes to ColumnStore")
    print("")
    
    emptyDatabase()
    
    print("creating dataframe 1: integers and their representation in ascii notation")
    asciiDF = sqlContext.createDataFrame(sc.parallelize(range(0, 128)).map(lambda i: Row(number=i, ascii_representation=chr(i)))).cache()
    asciiDFRows = asciiDF.count()
    asciiDFItems = asciiDFRows*len(asciiDF.columns)
    asciiDF.printSchema()
    print("bemchmarking dataframe 1")
    ascii_benchmark = benchmark1execution("ascii", asciiDF, "ascii_representation CHAR(1), number INT")
    asciiDF.unpersist()
    
    print("creating dataframe 2: two random generated doubles")
    randDF = sqlContext.range(0, 1000).withColumn('uniform', rand(seed=23)).withColumn('normal', randn(seed=42)).cache()
    randDFRows = randDF.count()
    randDFItems = randDFRows*len(randDF.columns)
    randDF.printSchema()
    print("bemchmarking dataframe 2")
    rand_benchmark = benchmark1execution("rand", randDF, "id BIGINT, uniform DOUBLE, normal DOUBLE")
    randDF.unpersist()
    
    print("creating dataframe 3: sha1, sha256, sha512 and md5 hashes of integers")
    tmpDF = sqlContext.createDataFrame(sc.parallelize(range(0, 1000)).map(lambda i: Row(number=i, string=str(i))))
    hashDF = tmpDF.select(tmpDF.number, sha1(tmpDF.string).alias("sha1"), sha2(tmpDF.string,256).alias("sha256"), sha2(tmpDF.string,512).alias("sha512"), md5(tmpDF.string).alias("md5")).cache()
    hashDFRows = hashDF.count()
    hashDFItems = hashDFRows*len(hashDF.columns)
    hashDF.printSchema()
    print("bemchmarking dataframe 3")
    hash_benchmark = benchmark1execution("hash", hashDF, "number BIGINT, sha1 VARCHAR(40), sha256 VARCHAR(64), sha512 VARCHAR(128), md5 VARCHAR(32)")
    hashDF.unpersist()
    
    print("jdbc_innodb\tjdbc_columnstore\tapi_columnstore\t\trows\t\titems")
    print("%.3fs\t\t%.3fs\t\t\t%.3fs\t\t\t%i\t\t%i" %(ascii_benchmark[0], ascii_benchmark[1], ascii_benchmark[2], asciiDFRows, asciiDFItems))
    print("%.3fs\t\t%.3fs\t\t\t%.3fs\t\t\t%i\t\t%i" %(rand_benchmark[0], rand_benchmark[1], rand_benchmark[2], randDFRows, randDFItems))
    print("%.3fs\t\t%.3fs\t\t\t%.3fs\t\t\t%i\t\t%i" %(hash_benchmark[0], hash_benchmark[1], hash_benchmark[2], hashDFRows, hashDFItems))
    
def benchmark1execution(name, dataframe, schema):
    t = time.time()
    dataframe.write.option("createTableOptions", "ENGINE=innodb").option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_innodb_%s" %(name,), properties=properties)
    jdbc_innodb_time = time.time() - t
    t = time.time()
    dataframe.write.option("numPartitions", 1).option("createTableOptions", "ENGINE=columnstore").option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_columnstore_%s" %(name,), properties=properties)
    jdbc_columnstore_time = time.time() - t
    t = time.time()
    createColumnStoreAPITable("api_columnstore_%s" %(name,), schema)
    columnStoreExporter.export("benchmark","api_columnstore_%s" %(name,),dataframe)
    api_columnstore_time = time.time() - t
    return jdbc_innodb_time, jdbc_columnstore_time, api_columnstore_time    
    
    
def benchmark2():
    print("===Benchmark 2===")
    print("Comparing JDBC writes to InnoDB and API writes to ColumnStore with larger datasets")
    print("")
    
    emptyDatabase()
    
    print("creating dataframe 1: two random generated doubles")
    randDF = sqlContext.range(0, 7000000).withColumn('uniform', rand(seed=23)).withColumn('normal', randn(seed=42)).cache()
    randDFRows = randDF.count()
    randDFItems = randDFRows*len(randDF.columns)
    randDF.printSchema()
    print("bemchmarking dataframe 1")
    rand_benchmark = benchmark2execution("rand", randDF, "id BIGINT, uniform DOUBLE, normal DOUBLE")
    randDF.unpersist()
    
    print("creating dataframe 2: sha1, sha256, sha512 and md5 hashes of integers")
    tmpDF = sqlContext.createDataFrame(sc.parallelize(range(0, 3000000)).map(lambda i: Row(number=i, string=str(i))))
    hashDF = tmpDF.select(tmpDF.number, sha1(tmpDF.string).alias("sha1"), sha2(tmpDF.string,256).alias("sha256"), sha2(tmpDF.string,512).alias("sha512"), md5(tmpDF.string).alias("md5")).cache()
    hashDFRows = hashDF.count()
    hashDFItems = hashDFRows*len(hashDF.columns)
    hashDF.printSchema()
    print("bemchmarking dataframe 2")
    hash_benchmark = benchmark2execution("hash", hashDF, "number BIGINT, sha1 VARCHAR(40), sha256 VARCHAR(64), sha512 VARCHAR(128), md5 VARCHAR(32)")
    hashDF.unpersist()
    
    print("jdbc_innodb\tapi_columnstore\t\trows\t\titems")
    print("%.3fs\t\t%.3fs\t\t%i\t\t%i" %(rand_benchmark[0], rand_benchmark[1], randDFRows, randDFItems))
    print("%.3fs\t\t%.3fs\t\t%i\t\t%i" %(hash_benchmark[0], hash_benchmark[1], hashDFRows, hashDFItems))
    
def benchmark2execution(name, dataframe, schema):
    t = time.time()
    dataframe.write.option("createTableOptions", "ENGINE=innodb")    .option("createTableColumnTypes", schema).jdbc(url, "benchmark.jdbc_innodb_%s" %(name,), properties=properties)
    jdbc_innodb_time = time.time() - t
    t = time.time()
    createColumnStoreAPITable("api_columnstore_%s" %(name,), schema)
    columnStoreExporter.export("benchmark","api_columnstore_%s" %(name,),dataframe)
    api_columnstore_time = time.time() - t
    return jdbc_innodb_time, api_columnstore_time
    
main()
