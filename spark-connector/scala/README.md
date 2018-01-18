# MariaDB ColumnStore API Scala Spark Connector
This provides a connector between the MariaDB ColumnStore API Java Wrapper and Spark.

Currently there is only one function.
```scala
ColumnStoreExporter.export("database", "table", DataFrame)
```
that exports a DataFrame to an existing table.

