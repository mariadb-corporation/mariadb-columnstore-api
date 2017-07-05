ColumnStoreBulkInsert Class
===========================

.. cpp:class:: ColumnStoreBulkInsert

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

setColumn()
-----------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, std::string& value, columnstore_data_convert_status_t* status = nullptr)
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
   :raises ColumnStoreException: If there is an error setting the column, such as truncation error when :cpp:func:`ColumnStoreBulkInsert::setTruncateIsError` is used or an invalid column number is supplied

setNull()
---------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::setNull(uint16_t columnNumber, columnstore_data_convert_status_t* status = nullptr)

   Sets a ``NULL`` for a given column.

   :param columnNumber: The column number to set (starting from ``0``)
   :param status: An optional pointer to a user supplied :cpp:type:`columnstore_data_convert_status_t` type. If supplied this will be set to the resulting status of any data conversion required.
   :returns: A pointer to the :cpp:class:`ColumnStoreBulkInsert` class so that calls can be chained
   :raises ColumnStoreException: If there is an error setting the column, such as an invalid column number is supplied

writeRow()
----------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreBulkInsert::writeRow()

   States that a row is ready to be written.

   .. note::
      The row may not be written at this stage. The library will batch an amount of rows together before sending them, by default data is only sent every 100,000 rows or :cpp:func::`commit` is called.

   :raises ColumnStoreException: If there has been an error during the write

commit()
--------

.. cpp:function:: void ColumnStoreBulkInsert::commit()

   Commits the data to the table.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :cpp:func:`ColumnStoreBulkInsert::getSummary`. Attempts to use it again will trigger an exception.

   :raises ColumnStoreException: If there has been an error during the commit

rollback()
----------

.. cpp:function:: void ColumnStoreBulkInsert::rollback()

   Rolls back the data written to the table.

   .. note::
      After making this call the transaction is completed and the class should not be used for anything but :cpp:func:`ColumnStoreBulkInsert::getSummary`. Attempts to use it again will trigger an exception.

   :raises ColumnStoreException: If there has been an error during the rollback

getSummary()
------------

.. cpp:function:: ColumnStoreSummary* ColumnStoreBulkInsert::getSummary()

   Gets a pointer to the summary information for this bulk write transaction.

   .. note::
      This is a pointer to an internal structure and should **not** be freed by the user application.

   :returns: A pointer the the summary information

setTruncateIsError()
--------------------

.. cpp:function:: void ColumnStoreBulkInsert::setTruncateIsError(bool set)

   Sets whether or not a truncation of CHAR/VARCHAR data is an error. It is disabled by default.

   :param set: true to enable, false to disable

setBatchSize()
--------------

.. cpp:function:: void ColumnStoreBulkInsert::setBatchSize(uint32_t batchSize)

   Future use, this has not been implemented yet
