ColumnStoreExporter Object
==========================

.. java:package:: com.mariadb.columnstore.api.connector
   :noindex:

Methods
-------
generateTableStatement
^^^^^^^^^^^^^^^^^^^^^^

.. java:method:: public String generateTableStatement(DataFrame dataFrame) throws IllegalArgumentException

   Returns a DML CREATE TABLE statement without database prefix based on the schema of the submitted DataFrame. The table name is set to "spark_export".

   :param dataFrame: The DataFrame from whom the structure for the generated table statement will be inferred.
   
.. java:method:: public String generateTableStatement(DataFrame dataFrame, String database) throws IllegalArgumentException

   Returns a DML CREATE TABLE statement with database prefix based on the schema of the submitted DataFrame. The table name is set to "spark_export".

   :param dataFrame: The DataFrame from whom the structure for the generated table statement will be inferred.
   :param database: The database name used in the generated table statement.
   
   .. note:: 
      The submitted database name will automatically be parsed into the `ColumnStore naming convention`_, if not already compatible.
   
.. java:method:: public String generateTableStatement(DataFrame dataFrame, String database, String table) throws IllegalArgumentException

   Returns a DML CREATE TABLE statement for database.table based on the schema of the submitted DataFrame.

   :param dataFrame: The DataFrame from whom the structure for the generated table statement will be inferred.
   :param database: The database name used in the generated table statement.
   :param table: The table name used in the generated table statement.
   
   .. note:: 
      The submitted database and table names will automatically be parsed into the `ColumnStore naming convention`_, if not already compatible.

.. java:method:: public String generateTableStatement(DataFrame dataFrame, String database, String table, bool determineTypeLength) throws IllegalArgumentException

   Returns a DML CREATE TABLE statement for database.table based on the schema (and content) of the submitted DataFrame.

   :param dataFrame: The DataFrame from whom the structure for the generated table statement will be inferred.
   :param database: The database name used in the generated table statement.
   :param table: The table name used in the generated table statement.
   :param determineTypeLength: If set to true the content of the DataFrame will be analysed to determine the best SQL datatype for each column. Otherwise reasonable default types will be used.
   
   .. note:: 
      The submitted database and table names will automatically be parsed into the `ColumnStore naming convention`_, if not already compatible.
   
export
^^^^^^

.. java:method:: public void export(String database, String table, DataFrame df) throws Exception

   Exports the given DataFrame into an existing ColumnStore database.table using the default Columnstore.xml configuration.
   
   :param database: The target database the DataFrame is exported into.
   :param table: The target table the DataFrame is exported into.
   :param df: The DataFrame to export.

   .. note::
      To guarantee that the DataFrame import into ColumnStore is a single transaction, that is rollbacked in case of error, the DataFrame is first collected at the Spark master and from there written to the ColumnStore system. Therefore, it needs to fit into the memory of the Spark master.

   .. note::
       The schema of the DataFrame to export and the ColumnStore table to import have to match. Otherwise, the import will fail.
   
.. java:method:: public void export(String database, String table, DataFrame df, String configuration) throws Exception

   Exports the given DataFrame into an existing ColumnStore database.table using a specific Columnstore.xml configuration.
   
   :param database: The target database the DataFrame is exported into.
   :param table: The target table the DataFrame is exported into.
   :param df: The DataFrame to export.
   :param configuration: Path to the Columnstore.xml configuration to use for the export.

   .. note:: 
      To guarantee that the DataFrame import into ColumnStore is a single transaction, that is rollbacked in case of error, the DataFrame is first collected at the Spark master and from there written to the ColumnStore system. Therefore, it needs to fit into the memory of the Spark master.

   .. note::
       The schema of the DataFrame to export and the ColumnStore table to import have to match. Otherwise, the import will fail.
   
exportFromWorkers
^^^^^^^^^^^^^^^^^

.. java:method:: public void exportFromWorkers(String database, String table, RDD rdd) throws Exception

   Exports the given RDD into an existing ColumnStore database.table from the worker nodes using the default Columnstore.xml configuration.
   
   :param database: The target database the RDD is exported into.
   :param table: The target table the RDD is exported into.
   :param rdd: The RDD to export.
   
   .. note:: 
      Each partition of the RDD is imported as single transaction into ColumnStore. In case of an error only partitions in which the error occurred are rolled back. Already committed partitions will remain in the database.

   .. note::
      The schema of the RDD to export and the ColumnStore table to import have to match. Otherwise, the import will fail.
   
.. java:method:: public void exportFromWorkers(String database, String table, RDD rdd, List<Int> partitions) throws Exception

   Exports the given partitions of the RDD into an existing ColumnStore database.table from the worker nodes using the default Columnstore.xml configuration.
   
   :param database: The target database the RDD is exported into.
   :param table: The target table the RDD is exported into.
   :param rdd: The RDD to export.
   :param partitions: List of partitions identified by their integer to be exported. If an empty List is submitted all partitions are exported.

   .. note::
      Each partition of the RDD is imported as single transaction into ColumnStore. In case of an error only partitions in which the error occurred are rolled back. Already committed partitions will remain in the database.

   .. note::
      The schema of the RDD to export and the ColumnStore table to import have to match. Otherwise, the import will fail.
   
.. java:method:: public void exportFromWorkers(String database, String table, RDD rdd, List<Int> partitions, String configuration) throws Exception

   Exports the given partitions of the RDD into an existing ColumnStore database.table from the worker nodes using a specific Columnstore.xml configuration.
   
   :param database: The target database the RDD is exported into.
   :param table: The target table the RDD is exported into.
   :param rdd: The RDD to export.
   :param partitions: List of partitions identified by their integer to be exported. If an empty List is submitted all partitions are exported.
   :param configuration: Path to the Columnstore.xml configuration to use for the export.

   .. note::
      Each partition of the RDD is imported as single transaction into ColumnStore. In case of an error only partitions in which the error occurred are rolled back. Already committed partitions will remain in the database.

   .. note::
      The schema of the RDD to export and the ColumnStore table to import have to match. Otherwise, the import will fail.

parseTableColumnNameToCSConvention
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. java:method:: public String parseTableColumnNameToCSConvention(String input)

   Parses the input String according to the `ColumnStore naming convention`_ and returns it.
   
   :param input: The String that is going to be parsed.
  
.. _`ColumnStore naming convention`: https://mariadb.com/kb/en/library/columnstore-naming-conventions/