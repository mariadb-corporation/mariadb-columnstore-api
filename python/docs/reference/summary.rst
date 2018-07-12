ColumnStoreSummary Class
========================

.. py:class:: ColumnStoreSummary

   A class containing the summary information for a transaction. An instance of this should be obtained from :py:meth:`ColumnStoreBulkInsert.getSummary`.

getExecutionTime()
------------------

.. py:method:: ColumnStoreSummary.getExecutionTime()

   Returns the total time for the transaction in seconds, from creation of the :py:class:`ColumnStoreBulkInsert` class until commit or rollback.

   :returns: The total execution time in seconds

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setColumn(0, 123456)
   bulkInsert.setColumn(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the execution time for the transaction
   execTime = summary.getExecutionTime()
   ...

getRowsInsertedCount()
----------------------

.. py:method:: ColumnStoreSummary.getRowsInsertedCount()

   Returns the number of rows inserted during the transaction or failed to insert for a rollback.

   :returns: The total number of rows

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setColumn(0, 123456)
   bulkInsert.setColumn(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the number of inserted rows before they were rolled back
   rows = summary.getRowsInsertedCount()
   ...

getTruncationCount()
--------------------

.. py:method:: ColumnStoreSummary.getTruncationCount()

   Returns the number of truncated CHAR/VARCHAR values during the transaction.

   :returns: The total number of truncated values

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setColumn(0, 123456)
   bulkInsert.setColumn(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the number of truncated values before they were rolled back
   truncateCount = summary.getTruncationCount()
   ...

getSaturatedCount()
-------------------

.. py:method:: ColumnStoreSummary.getSaturatedCount()

   Returns the number of saturated values during the transaction.

   :returns: The total number of saturated values

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setColumn(0, 123456)
   bulkInsert.setColumn(1, 2147483650)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the number of saturated values before they were rolled back
   saturatedCount = summary.getSaturatedCount()
   ...


getInvalidCount()
-----------------

.. py:method:: ColumnStoreSummary.getInvalidCount()

   Returns the number of invalid values during the transaction.

   .. note::
      An invalid value is one where a data conversion during :py:meth:`ColumnStoreBulkInsert.setValue` was not possible. When this happens a ``0`` or empty string is used instead and the status value set accordingly.

   :returns: The total number of invalid values

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setColumn(0, 123456);
   # This is a DATE column, which is invalid to set as a date.
   # The result will be the date set to '0000-00-00'
   # and a invalid counter increment
   bulkInsert.setColumn(1, 123456)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the number of invalid values before they were rolled back
   invalidCount = summary.getInvalidCount()
   ...
