columnstore_data_convert_status_t Type
======================================

.. cpp:type:: columnstore_data_convert_status_t

   The status value used in :cpp:func:`ColumnStoreBulkInsert::setColumn` and :cpp:func:`ColumnStoreBulkInsert::setNull` to signify the status of any data conversion that occurred during setting.

.. cpp:type:: CONVERT_STATUS_NONE

   There was no problems during the conversion or no conversion.

.. cpp:type:: CONVERT_STATUS_SATURATED

   The value was saturated during the conversion, the maximum/minimum was used instead.

.. cpp:type:: CONVERT_STATUS_INVALID

   The value was invalid during the conversion, 0 or empty string was used instead.

.. cpp:type:: CONVERT_STATUS_TRUNCATED

   The value was truncated.

columnstore_data_types_t Type
=============================

.. cpp:type:: columnstore_data_types_t

   The data type as returned by :cpp:func:`ColumnStoreSystemCatalogColumn::getType`.

.. cpp:type:: DATA_TYPE_BIT

   BIT data type

.. cpp:type:: DATA_TYPE_TINYINT

   TINYINT data type

.. cpp:type:: DATA_TYPE_CHAR

   CHAR data type

.. cpp:type:: DATA_TYPE_SMALLINT

   SMALLINT data type

.. cpp:type:: DATA_TYPE_DECIMAL

   DECIMAL data type

.. cpp:type:: DATA_TYPE_MEDINT

   MEDIUMINT data type

.. cpp:type:: DATA_TYPE_INT

   INT data type

.. cpp:type:: DATA_TYPE_FLOAT

   FLOAT data type

.. cpp:type:: DATA_TYPE_DATE

   DATE data type

.. cpp:type:: DATA_TYPE_BIGINT

   BIGINT data type

.. cpp:type:: DATA_TYPE_DOUBLE

   DOUBLE data type

.. cpp:type:: DATA_TYPE_DATETIME

   DATETIME data type

.. cpp:type:: DATA_TYPE_VARCHAR

   VARCHAR data type

.. cpp:type:: DATA_TYPE_VARBINARY

   VARBINARY data type

.. cpp:type:: DATA_TYPE_CLOB

   Unused

.. cpp:type:: DATA_TYPE_BLOB

   BLOB data type

.. cpp:type:: DATA_TYPE_UTINYINT

   UNSIGNED TINYINT data type

.. cpp:type:: DATA_TYPE_USMALLINT

   UNSIGNED SMALLINT data type

.. cpp:type:: DATA_TYPE_UDECIMAL

   UNSIGNED DECIMAL data type

.. cpp:type:: DATA_TYPE_UMEDINT

   UNSIGNED MEDIUMINT data type

.. cpp:type:: DATA_TYPE_UINT

   UNSIGNED INT data type

.. cpp:type:: DATA_TYPE_UFLOAT

   UNSIGNED FLOAT data type

.. cpp:type:: DATA_TYPE_UBIGINT

   UNSIGNED BIGINT data type

.. cpp:type:: DATA_TYPE_UDOUBLE

   UNSIGNED DOUBLE data type

.. cpp:type:: DATA_TYPE_TEXT

   TEXT data type

columnstore_lock_types_t Type
=============================

   These are constants for the table locks as returned by :cpp:func:`TableLockInfo::state`
   
.. cpp:type:: LOCK_TYPE_CLEANUP

.. cpp:type:: LOCK_TYPE_LOADING

TableLockInfo Struct
====================

.. cpp:type:: TalbeLockInfo

   A struct containing table lock information

.. cpp:type:: TableLockInfo::id

   UNSIGNED int64 lock id

.. cpp:type:: TableLockInfo::ownerName

   std::string name of the owner of the table lock

.. cpp:type:: TableLockInfo::ownerPID

   UNSINGNED int32 process id of the lock owner

.. cpp:type:: TableLockInfo::ownerSessionID

   UNSIGNED int32 session id of the lock owner

.. cpp:type:: TableLockInfo::ownerTxnID

   UNSIGNED int32 transaction id of the lock owner

.. cpp:type:: TableLockInfo::state

   columnstore_lock_types_t state of the lock

.. cpp:type:: TableLockInfo::creationTime

   time_t creation time of the lock

.. cpp:type:: TableLockInfo::dbrootList

   std::vector<uint32_t> list of PMs involved in this lock


ColumnStoreDateTime Class
=========================

.. cpp:class:: ColumnStoreDateTime

   A class which is used to contain a date/time used to set ``DATE`` or ``DATETIME`` columns using :cpp:func:`ColumnStoreBulkInsert::setColumn`

ColumnStoreDateTime()
---------------------

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime()

   Sets the date/time to ``0000-00-00 00:00:00``.

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime(tm& time)

   Sets the date/time the value of the :cpp:type:`tm` struct.

   :param time: The date/time to set
   :raises ColumnStoreDataError: When an invalid date or time is supplied

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime(const std::string& dateTime, const std::string& format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :raises ColumnStoreDataError: When an invalid date or time is supplied

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t microsecond)

   Sets the date/time based on a given set of intergers

   .. note::
      Microseconds are for future usage, they are not currently supported in ColumnStore.

   :param year: The year
   :param month: The month of year
   :param day: The day of month
   :param hour: The hour
   :param minute: The minute
   :param second: The second
   :param microsecond: The microseconds
   :raises ColumnStoreDataError: When an invalid date or time is supplied

set()
-----

.. cpp:function:: bool ColumnStoreDateTime::set(tm& time)

   Sets the date/time using the value of the :cpp:type:`tm` struct.

   :param time: The date/time to set
   :returns: ``true`` if the date/time is valid, ``false`` if it is not

.. cpp:function:: bool ColumnStoreDateTime::set(const std::string& dateTime, const std::string& format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :returns: ``true`` if the date/time is valid, ``false`` if it is not

ColumnStoreDecimal Class
========================

.. cpp:class:: ColumnStoreDecimal

   A class which is used to contain a non-lossy decimal format used to set ``DECIMAL`` columns using :cpp:func:`ColumnStoreBulkInsert::setColumn`.

ColumnStoreDecimal()
--------------------

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal()

   Sets the decimal to ``0``.

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(int64_t value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set
   :raises ColumnStoreDataError: When an invalid value is supplied

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(const std::string& value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   :param value: The value to set
   :raises ColumnStoreDataError: When an invalid value is supplied

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(double value)

   Sets the decimal to the contents of a supplied :c:type:`double` value.

   .. note::
      The internally this uses the :cpp:type:`std::string` method so the performance may be lower than expected.

   :param value: The value to set
   :raises ColumnStoreDataError: When an invalid value is supplied

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(int64_t number, uint8_t scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number
   :raises ColumnStoreDataError: When an invalid number/scale is supplied

set()
-----

.. cpp:function:: bool ColumnStoreDecimal::set(int64_t value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set
   :returns: Always returns ``true``

.. cpp:function:: bool ColumnStoreDecimal::set(const std::string& value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   :param value: The value to set
   :returns: ``true`` if the conversion was successful or ``false`` if it failed

.. cpp:function:: bool ColumnStoreDecimal::set(double value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   .. note::
      The internally this uses the :cpp:type:`std::string` method so the performance may be lower than expected.

   :param value: The value to set
   :returns: ``true`` if the conversion was successful or ``false`` if it failed

.. cpp:function:: bool ColumnStoreDecimal::set(int64_t number, uint8_t scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number
   :returns: ``true`` if the conversion was successful or ``false`` if it failed


ColumnStoreTime Class
=====================

.. cpp:class:: ColumnStoreTime

   A class which is used to contain a date/time used to set ``TIME`` columns using :cpp:func:`ColumnStoreBulkInsert::setColumn`

ColumnStoreTime()
-----------------

.. cpp:function:: ColumnStoreTime::ColumnStoreTime()

   Sets the date/time to ``00:00:00``.

.. cpp:function:: ColumnStoreTime::ColumnStoreTime(tm& time)

   Sets the time value of the :cpp:type:`tm` struct.

   :param time: The time to set
   :raises ColumnStoreDataError: When an invalid date or time is supplied

.. cpp:function:: ColumnStoreTime::ColumnStoreTime(const std::string& time, const std::string& format)

   Sets the time based on a given string and format.

   :param time: A string containing the time to set
   :param format: The format specifier for the time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :raises ColumnStoreDataError: When an invalid date or time is supplied

.. cpp:function:: ColumnStoreTime::ColumnStoreTime(int32_t hour, uint32_t minute, uint32_t second, uint32_t microsecond = 0, bool is_negative = false)

   Sets the time based on a given set of intergers

   .. note::
      If the the time is a negative and the hours are zero then ``is_negative`` should be set. Otherwise the driver will automatically set this for you.

   :param hour: The hour
   :param minute: The minute
   :param second: The second
   :param microsecond: The microseconds
   :param is_negative: A zero hour time that is negative
   :raises ColumnStoreDataError: When an invalid date or time is supplied

set()
-----

.. cpp:function:: bool ColumnStoreTime::set(tm& time)

   Sets the time using the value of the :cpp:type:`tm` struct.

   :param time: The time to set
   :returns: ``true`` if the time is valid, ``false`` if it is not

.. cpp:function:: bool ColumnStoreTime::set(const std::string& time, const std::string& format)

   Sets the time based on a given string and format.

   :param time: A string containing the time to set
   :param format: The format specifier for the time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :returns: ``true`` if the time is valid, ``false`` if it is not

ColumnStoreSystemCatalog Class
==============================

.. cpp:class:: ColumnStoreSystemCatalog

   A class which contains the ColumnStore system catalog of tables and columns. It should be instantiated using :cpp:func:`ColumnStoreDriver::getSystemCatalog`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.

getTable()
----------

.. cpp:function:: ColumnStoreSystemCatalogTable& ColumnStoreSystemCatalog::getTable(const std::string& schemaName, const std::string& tableName)

   Gets the table information for a specific table.

   :param schemaName: The schema the table is in
   :param tableName: The name of the table
   :returns: The table information
   :raises ColumnStoreNotFound: If the table is not found in the system catalog

ColumnStoreSystemCatalogTable Class
===================================

.. cpp:class:: ColumnStoreSystemCatalogTable

   A class which contains the system catalog information for a specific table. It should be instantiated using :cpp:func:`ColumnStoreSystemCatalog::getTable`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.


getSchemaName()
---------------

.. cpp:function:: const std::string& ColumnStoreSystemCatalogTable::getSchemaName()

   Retrieves the database schema name for the table

   :returns: The schema name

getTableName()
--------------

.. cpp:function:: const std::string& ColumnStoreSystemCatalogTable::getTableName()

   Retrieves the table name for the table

   :returns: The table name

getOID()
--------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogTable::getOID()

   Retrieves the ColumnStore object ID for the table.

   :returns: The object ID for the table

getColumnCount()
----------------

.. cpp:function:: uint16_t ColumnStoreSystemCatalogTable::getColumnCount()

   Retrieves the number of columns in the table

   :returns: The number of columns in the table

getColumn()
-----------

.. cpp:function:: ColumnStoreSystemCatalogColumn& ColumnStoreSystemCatalogTable::getColumn(const std::string& columnName)

   Retrieves the column information for a specified column by name

   :param columnName: The name of the column to retrieve
   :returns: The column information
   :raises ColumnStoreNotFound: If the column is not found

.. cpp:function:: ColumnStoreSystemCatalogColumn& ColumnStoreSystemCatalogTable::getColumn(uint16_t columnNumber)

   Retrieves the column information for a specified column by number starting at zero

   :param columnNumber: The number of the column to retrieve starting at ``0``
   :returns: The column information
   :raises ColumnStoreNotFound: If the column is not found


ColumnStoreSystemCatalogColumn Class
====================================

.. cpp:class:: ColumnStoreSystemCatalogColumn

   A class containing information about a specific column in the system catalog. Should be instantiated using :cpp:func:`ColumnStoreSystemCatalogTable::getColumn`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.


getOID()
--------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getOID()

   Retrieves the ColumnStore object ID for the column

   :returns: The column object ID

getColumnName()
---------------

.. cpp:function:: const std::string& ColumnStoreSystemCatalogColumn::getColumnName()

   Retrieves the name of the column

   :returns: The column name

getDictionaryOID()
------------------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getDictionaryOID()

   Retrieves the dictionary object ID for the column (or ``0`` if there is no dictionary)

   :returns: The dictionary object ID or ``0`` for no dictionary

getType()
---------

.. cpp:function:: columnstore_data_types_t ColumnStoreSystemCatalogColumn::getType()

   Retrieves the data type for the column

   :returns: The data type for the column

getWidth()
----------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getWidth()

   Retrieves the width in bytes for the column

   :returns: The width in bytes

getPosition()
-------------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getPosition()

   Retrieves the column's position in the table. The sequence of columns in the table is sorted on object ID, columns may be out-of-order if an ALTER TABLE has inserted one in the middle of the table.

   :returns: The column's position in the table

getDefaultValue()
-----------------

.. cpp:function:: const std::string& ColumnStoreSystemCatalogColumn::getDefaultValue()

   Retrieves the default value for the column in text. The value is empty for no default.

   :returns: The column's default value

isAutoincrement()
-----------------

.. cpp:function:: bool ColumnStoreSystemCatalogColumn::isAutoincrement()

   Retrieves whether or not this column is an autoincrement column.

   :returns: ``true`` if this column is autoincrement, ``false`` if it isn't

getPrecision()
--------------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getPrecision()

   Retrieves the decimal precision for the column.

   :returns: The decimal precision

getScale()
----------

.. cpp:function:: uint32_t ColumnStoreSystemCatalogColumn::getScale()

   Retrieves the decimal scale for the column.

   :returns: The decimal scale

isNullable()
------------

.. cpp:function:: bool ColumnStoreSystemCatalogColumn::isNullable()

   Retrieves whether or not the column can be set to ``NULL``

   :returns: ``true`` if the column can be ``NULL`` or ``false`` if it can not

compressionType()
-----------------

.. cpp:function:: uint8_t ColumnStoreSystemCatalogColumn::compressionType()

   Retrieves the compression type for the column. ``0`` means no compression and ``2`` means Snappy compression

   :returns: The compression type for the column

