# MariaDB ColumnStore API Scala Spark Connector
This provides a connector between the MariaDB ColumnStore API Java Wrapper and Spark.

Currently there are two functions.
```scala
ColumnStoreExporter.export("database", "table", DataFrame, [path to Columnstore.xml])
ColumnStoreExporter.generateTableStatement(DataFrame, ["database", "table", determineTypeLength])
```
export() exports a DataFrame to an existing table, and 
generateTableStatement() generates a CREATE TABLE SQL statement based on the schema of the DataFrame to export.

## Benchmarking
We included a benchmark to compare ColumnStoreExporter's write capabilities with JDBC's write capabilities to ColumnStore. A second benchmark compares ColumnStoreExporter writing to ColumnStore with JDBC writing to InnoDB. To execute the benchmark simply type:
```shell
./gradlew benchmark
```
**NOTE** A machine with at least 16GiB of memory is advised to use for benchmarking.
