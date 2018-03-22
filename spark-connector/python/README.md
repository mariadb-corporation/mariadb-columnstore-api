# MariaDB ColumnStore API Python Spark Connector
This provides a connector between the MariaDB ColumnStore API Python Wrapper and Spark.

Currently there is only one function.
```python
columnStoreExporter.export("database", "table", DataFrame, [path to Columnstore.xml])
```
that exports a DataFrame to an existing table.

### Testing dependencies
Python's testing dependencies can be found in test/requirements.txt and can be installed via:
```shell
sudo pip install -r test/requirements.txt
```

### Benchmarking
We included a benchmark to compare ColumnStoreExporter's write capabilities with JDBC's write capabilities to ColumnStore. A second benchmark compares ColumnStoreExporter writing to ColumnStore with JDBC writing to InnoDB. The benchmark requires that a mariadb-java-client.jar is available. To execute the benchmark simply type:
```shell
./benchmark.sh PATH_TO_MARIADB_JDBC_CLIENT_JAR
```
e.g.
```shell
./benchmark.sh /usr/local/lib/mariadb-java-client-2.2.0.jar
```
**NOTE** A machine with at least 16GiB of memory is advised to use for benchmarking.
