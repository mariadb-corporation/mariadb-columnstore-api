ColumnStoreBulkInsert Class
===========================

.. py:class:: ColumnStoreBulkInsert

   The bulk insert class is designed to rapidly insert data into a ColumnStore installation.

   .. note::
      An instance of this class should only be created from :py:class:`ColumnStoreDriver`

   .. note::
      If an explicit commit is not given before the class is destroyed then an implicit rollback will be executed

   .. note::
      This class should be viewed as a single transaction. Once committed or rolled back the class cannot be used for any more operations beyond getting the summary. Further usage attempts will result in an exception being thrown.

getColumnCount()
----------------

.. py:method:: ColumnStoreBulkInsert.getColumnCount()

   Gets the number of columns in the table to be inserted into.

   :returns: A count of the number of columns

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   pymcsapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)
   # columnCount will now contain the number of columns in the table
   columnCount = bulkInsert.getColumnCount()
   ...


setColumn()
-----------

.. py:method:: ColumnStoreBulkInsert.setColumn(columnNumber, value)

   Sets a value for a given column.

   :param columnNumber: The column number to set (starting from ``0``)
   :param value: The value to set this column
   :returns: A List of a pointer to the :py:class:`ColumnStoreBulkInsert` class so that calls can be chained, and the return status.
   :raises RuntimeError: If there is an error setting the column, such as truncation error when :py:meth:`ColumnStoreBulkInsert.setTruncateIsError` is used or an invalid column number is supplied
   :raises RuntimeError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Create a decimal value
   decimalVal = pymcsapi.ColumnStoreDecimal("3.14159")

   # And an int value
   intVal = 123456

   # And a string value
   strVal = "Hello World"

   # Finally a date/time values
   dateTime = pymcsapi.ColumnStoreDateTime("1999-01-01 23:23:23")

   nxt, status = bulkInsert.setColumn(0, intVal)
   # Check conversion status
   if (status != CONVERT_STATUS_NONE):
       return 1
   nxt, status = bulkInsert.setColumn(1, decimalVal)
   # Check conversion status
   if (status != CONVERT_STATUS_NONE):
       return 1
   nxt, status = bulkInsert.setColumn(2, strVal)
   # Check conversion status
   if (status != CONVERT_STATUS_NONE):
   	   return 1
   nxt, status = bulkInsert.setColumn(3, dateTime)
   # Check conversion status
   if (status != CONVERT_STATUS_NONE):
       return 1

   # Write this row ready to start another
   bulkInsert.writeRow()

   decimalVal.set("1.41421")
   intVal = 654321
   strVal = "dlroW olleH"
   dateTime.set("2017-07-05 22:00:43")

   # A chained example
   bulkInsert.setColumn(0, intVal)[0].setColumn(1, decimalVal)[0].setColumn(2, strVal)[0].setColumn(3, dateTime)[0].writeRow()
   ...


setNull()
---------

.. py:method:: ColumnStoreBulkInsert.setNull(columnNumber)

   Sets a ``NULL`` for a given column.

   :param columnNumber: The column number to set (starting from ``0``)
   :returns: A List of a pointer to the :py:class:`ColumnStoreBulkInsert` class so that calls can be chained, and the return status
   :raises RuntimeError: If there is an error setting the column, such as an invalid column number is supplied
   :raises RuntimeError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set an whole row of NULLs
   bulkInsert.setNull(0)[0].setNull(1)[0].setNull(2)[0].setNull(3)[0].writeRow()
   ...

resetRow()
----------

.. py:method:: ColumnStoreBulkInsert.resetRow()

   Resets everything that has been set for the current row. This method should be used to clear the row memory without using :py:meth:`ColumnStoreBulkInsert.writeRow`.

   :raises RuntimeError: If the transaction has already been closed

writeRow()
----------

.. py:method:: ColumnStoreBulkInsert.writeRow()

   States that a row is ready to be written.

   .. note::
      The row may not be written at this stage. The library will batch an amount of rows together before sending them, by default data is only sent to the server every 100,000 rows or :py:meth:`ColumnStoreBulkInsert.commit` is called. Data is not committed with ``writeRow()``, it has to be explicitly committed at the end of the transaction. 

   :raises RuntimeError: If there has been an error during the write at the network level
   :raises RuntimeError: If there has been an error during the write at the remote server level
   :raises RuntimeError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setValue(0, 123456)
   bulkInsert->setValue(1, 654321)

   # Write the row
   bulkInsert.writeRow()
   ...


commit()
--------

.. py:method:: ColumnStoreBulkInsert.commit()

   Commits the data to the table.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :py:meth:`ColumnStoreBulkInsert.getSummary` or :py:meth:`ColumnStoreBulkInsert.isActive`. Attempts to use it again will trigger an exception.

   .. note::
      If the commit fails a rollback will be executed automatically upon deletion of the :py:class:`ColumnStoreBulkInsert` object.

   :raises RuntimeError: If there has been an error during the write at the network level
   :raises RuntimeError: If there has been an error during the write at the remote server level
   :raises RuntimeError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setValue(0, 123456)
   bulkInsert.setValue(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Commit the transaction
   bulkInsert.commit()

   # This WILL throw an exception if uncommented
   # bulkInsert.setValue(0, 99999)
   ...


rollback()
----------

.. py:method:: ColumnStoreBulkInsert.rollback()

   Rolls back the data written to the table. If the transaction has already been committed or rolled back this will just return without error.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :py:meth:`ColumnStoreBulkInsert.getSummary` or :py:meth:`ColumnStoreBulkInsert.isActive`. Attempts to use it again will trigger an exception.

   :raises RuntimeError: If there has been an error during the write at the network level
   :raises RuntimeError: If there has been an error during the write at the remote server level

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setValue(0, 123456)
   bulkInsert.setValue(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # This WILL throw an exception if uncommented
   # bulkInsert.setValue(0, 99999)
   ...


isActive()
----------

.. py:method:: ColumnStoreBulkInsert.isActive()

   Returns whether or not the bulk insert transaction is still active.

   :returns: True if the transaction is still active, False if it has been committed or rolled back

getSummary()
------------

.. py:method:: ColumnStoreBulkInsert.getSummary()

   Gets the summary information for this bulk write transaction.

   :returns: The summary object

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # Set values for a 2 int column table
   bulkInsert.setValue(0, 123456)
   bulkInsert.setValue(1, 654321)

   # Write the row
   bulkInsert.writeRow()

   # Rollback the transaction
   bulkInsert.rollback()

   # Get the summary
   summary = bulkInsert.getSummary()

   # Get the number of inserted rows before they were rolled back
   rows = summary.getRowsInsertedCount()
   ...


setTruncateIsError()
--------------------

.. py:method:: ColumnStoreBulkInsert::setTruncateIsError(set)

   Sets whether or not a truncation of CHAR/VARCHAR data is an error. It is disabled by default.

   :param set: True to enable, False to disable

Example
^^^^^^^

This example can be used inside the try...except blocks in the :py:class:`ColumnStoreDriver` examples.

.. code-block:: python
   :linenos:

   ...
   driver = pymcapi.ColumnStoreDriver()
   bulkInsert = driver.createBulkInsert(db, table, 0, 0)

   # A short string that will insert fine
   bulkInsert.setValue(0, "Short string")

   # This long string will truncate on my VARCHAR(20) and throw an exception
   bulkInsert.setValue(1, "This is a long string test to demonstrate setTruncateIsError()")
   ...

setBatchSize()
--------------

.. py:method:: ColumnStoreBulkInsert.setBatchSize(batchSize)

   Future use, this has not been implemented yet
