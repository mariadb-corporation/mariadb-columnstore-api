ColumnStoreBulkInsert Class
===========================

.. py:class:: ColumnStoreBulkInsert

   The bulk insert class is designed to rapidly insert data into a ColumnStore installation.

   .. note::
      An instance of this class should only be created from :cpp:class:`ColumnStoreDriver`

   .. note::
      If an explicit commit is not given before the class is destroyed then an implicit rollback will be executed

   .. note::
      This class should be viewed as a single transaction. Once committed or rolled back the class cannot be used for any more operations beyond getting the summary. Further usage attempts will result in an exception being thrown.

getColumnCount()
----------------

.. cpp:function:: uint16_t ColumnStoreBulkInsert::getColumnCount()

   Gets the number of columns in the table to be inserted into.

   :returns: A count of the number of columns

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);
   // columnCount will now contain the number of columns in the table
   uint16_t columnCount = bulkInsert->getColumnCount();
   ...


setColumn()
-----------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, const std::string& value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint64_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int64_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint32_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int32_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint16_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int16_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint8_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int8_t value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, double value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDateTime& value, columnstore_data_convert_status_t* status = nullptr)
.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDecimal& value, columnstore_data_convert_status_t* status = nullptr)

   Sets a value for a given column.

   :param columnNumber: The column number to set (starting from ``0``)
   :param value: The value to set this column
   :param status: An optional pointer to a user supplied :cpp:type:`columnstore_data_convert_status_t` type. If supplied this will be set to the resulting status of any data conversion required.
   :returns: A pointer to the :cpp:class:`ColumnStoreBulkInsert` class so that calls can be chained
   :raises ColumnStoreDataError: If there is an error setting the column, such as truncation error when :cpp:func:`ColumnStoreBulkInsert::setTruncateIsError` is used or an invalid column number is supplied
   :raises ColumnStoreUsageError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Create a decimal value
   ColumnStoreDecimal decimalVal;
   decimalVal.set("3.14159");

   // And an int value
   uint32_t intVal = 123456;

   // And a string value
   std::string strVal("Hello World");

   // Finally a date/time values
   ColumnStoreDateTime dateTime;
   std::string newTime("1999-01-01 23:23:23");
   std::string tFormat("%Y-%m-%d %H:%M:%S");
   dateTime.set(newTime, tFormat);

   // A status variable so we can check all is good
   mcsapi::columnstore_data_convert_status_t status;

   bulkInsert->setColumn(0, intVal, &status);
   // Check conversion status
   if (status != CONVERT_STATUS_NONE)
   	return 1;
   bulkInsert->setColumn(1, decimalVal, &status);
   // Check conversion status
   if (status != CONVERT_STATUS_NONE)
   	return 1;
   bulkInsert->setColumn(2, strVal, &status);
   // Check conversion status
   if (status != CONVERT_STATUS_NONE)
   	return 1;
   bulkInsert->setColumn(3, dateTime, &status);
   // Check conversion status
   if (status != CONVERT_STATUS_NONE)
   	return 1;

   // Write this row ready to start another
   bulkInsert->writeRow();

   decimalVal.set("1.41421");
   intVal = 654321;
   strVal = "dlroW olleH";
   newTime = "2017-07-05 22:00:43";
   dateTime.set(newTime, tFormat);

   // A chained example
   bulkInsert->setColumn(0, intVal)->setColumn(1, decimalVal)->setColumn(2, strVal)->setColumn(3, dateTime)->writeRow();
   ...


setNull()
---------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setNull(uint16_t columnNumber, columnstore_data_convert_status_t* status = nullptr)

   Sets a ``NULL`` for a given column.

   :param columnNumber: The column number to set (starting from ``0``)
   :param status: An optional pointer to a user supplied :cpp:type:`columnstore_data_convert_status_t` type. If supplied this will be set to the resulting status of any data conversion required.
   :returns: A pointer to the :cpp:class:`ColumnStoreBulkInsert` class so that calls can be chained
   :raises ColumnStoreDataError: If there is an error setting the column, such as an invalid column number is supplied
   :raises ColumnStoreUsageError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set an whole row of NULLs
   bulkInsert->setNull(0)->setNull(1)->setNull(2)->setNull(3)->writeRow();
   ...

resetRow()
----------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::resetRow()

   Resets everything that has been set for the current row. This method should be used to clear the row memory without using :cpp:func:`ColumnStoreBulkInsert::writeRow`.

   :raises ColumnStoreUsageError: If the transaction has already been closed

writeRow()
----------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::writeRow()

   States that a row is ready to be written.

   .. note::
      The row may not be written at this stage. The library will batch an amount of rows together before sending them, by default data is only sent to the server every 100,000 rows or :cpp:func:`ColumnStoreBulkInsert::commit` is called. Data is not committed with ``writeRow()``, it has to be explicitly committed at the end of the transaction. 

   :raises ColumnStoreNetworkError: If there has been an error during the write at the network level
   :raises ColumnStoreServerError: If there has been an error during the write at the remote server level
   :raises ColumnStoreUsageError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();
   ...


commit()
--------

.. cpp:function:: void ColumnStoreBulkInsert::commit()

   Commits the data to the table.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :cpp:func:`ColumnStoreBulkInsert::getSummary` or :cpp:func:`ColumnStoreBulkInsert::isActive`. Attempts to use it again will trigger an exception.

   .. note::
      If the commit fails a rollback will be executed automatically upon deletion of the :cpp:class:`ColumnStoreBulkInsert` object.

   :raises ColumnStoreNetworkError: If there has been an error during the write at the network level
   :raises ColumnStoreServerError: If there has been an error during the write at the remote server level
   :raises ColumnStoreUsageError: If the transaction has already been closed

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();

   // Commit the transaction
   bulkInsert->commit();

   // This WILL throw an exception if uncommented
   // bulkInsert->setValue(0, (uint32_t) 99999);
   ...


rollback()
----------

.. cpp:function:: void ColumnStoreBulkInsert::rollback()

   Rolls back the data written to the table. If the transaction has already been committed or rolled back this will just return without error.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :cpp:func:`ColumnStoreBulkInsert::getSummary` or :cpp:func:`ColumnStoreBulkInsert::isActive`. Attempts to use it again will trigger an exception.

   :raises ColumnStoreNetworkError: If there has been an error during the write at the network level
   :raises ColumnStoreServerError: If there has been an error during the write at the remote server level

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // This WILL throw an exception if uncommented
   // bulkInsert->setValue(0, (uint32_t) 99999);
   ...


isActive()
----------

.. cpp:function:: bool ColumnStoreBulkInsert::isActive()

   Returns whether or not the bulk insert transaction is still active.

   :returns: true if the transaction is still active, false if it has been committed or rolled back

getSummary()
------------

.. cpp:function:: ColumnStoreSummary& ColumnStoreBulkInsert::getSummary()

   Gets the summary information for this bulk write transaction.

   :returns: The summary object

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // Get the summary
   ColumnStoreSummary summary = bulkInsert->getSummary();

   // Get the number of inserted rows before they were rolled back
   uint64_t rows = summary.getRowsInsertedCount();
   ...


setTruncateIsError()
--------------------

.. cpp:function:: void ColumnStoreBulkInsert::setTruncateIsError(bool set)

   Sets whether or not a truncation of CHAR/VARCHAR data is an error. It is disabled by default.

   :param set: true to enable, false to disable

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   bulkInsert->setTruncateIsError(true);
   std::string strVal("Short string");

   // A short string that will insert fine
   bulkInsert->setValue(0, strVal);

   // This long string will truncate on my VARCHAR(20) and throw an exception
   strVal = "This is a long string test to demonstrate setTruncateIsError()";
   bulkInsert->setValue(1, strVal);
   ...

setBatchSize()
--------------

.. cpp:function:: void ColumnStoreBulkInsert::setBatchSize(uint32_t batchSize)

   Future use, this has not been implemented yet
