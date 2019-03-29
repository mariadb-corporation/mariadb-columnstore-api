columnStoreExporter Module
==========================

.. py:module:: columnStoreExporter

Functions
---------
generateTableStatement
^^^^^^^^^^^^^^^^^^^^^^

.. py:function:: columnStoreExporter.generateTableStatement(dataFrame, database=None, table="spark_export", determineTypeLengths=False)

   Generates a CREATE TABLE statement based on the schema of the submitted DataFrame.

   :returns: A DML CREATE TABLE statement based on the schema of the submitted DataFrame.
   :param dataFrame: The DataFrame from whom the structure for the generated table statement will be inferred.
   :param database: The database name used in the generated table statement.
   :param table: The table name used in the generated table statement.
   :param determineTypeLength: If set to True the content DataFrame will be analysed to determine the best SQL datatype for each column. Otherwise reasonable default types will be used.
   
   .. note::
      The submitted database and table names will automatically be parsed into the `ColumnStore naming convention`_, if not already compatible.
   
export
^^^^^^

.. py:function:: columnStoreExporter.export(database, table, df, configuration=None)

   Exports the given DataFrame into an existing ColumnStore table.
   
   :param database: The target database the DataFrame is exported into.
   :param table: The target table the DataFrame is exported into.
   :param df: The DataFrame to export.
   :param configuration: Path to the Columnstore.xml configuration to use for the export. If None is given, the default Columnstore.xml will be used.

   .. note::
      To guarantee that the DataFrame import into ColumnStore is a single transaction, that is rollbacked in case of error, the DataFrame is first collected at the Spark master and from there written to the ColumnStore system. Therefore, it needs to fit into the memory of the Spark master.

   .. note::
       The schema of the DataFrame to export and the ColumnStore table to import have to match. Otherwise, the import will fail.

parseTableColumnNameToCSConvention
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. py:function:: columnStoreExporter.parseTableColumnNameToCSConvention(input)

   Parses the input String according to the `ColumnStore naming convention`_ and returns it.
   
   :returns: The parsed input String in ColumnStore naming convention.
   :param input: The String that is going to be parsed.
  
.. _`ColumnStore naming convention`: https://mariadb.com/kb/en/library/columnstore-naming-conventions/