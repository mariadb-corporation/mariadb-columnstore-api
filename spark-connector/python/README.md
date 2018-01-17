# MariaDB ColumnStore API Python Spark Connector
This provides a connector between the MariaDB ColumnStore API Python Wrapper and Spark.

Currently there is only one function.
```python
sparkPythonMcsapiConnector.export("database", "table", DataFrame)
```
that exports a DataFrame to an existing table.

### Testing dependencies
Python's testing dependencies can be found in test/requirements.txt and can be installed via:
```shell
sudo pip install -r test/requirements.txt
```
