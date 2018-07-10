columnstore_data_convert_status constants
=========================================

   These are the constants for the status values used in :py:meth:`ColumnStoreBulkInsert.setColumn` and :py:meth:`ColumnStoreBulkInsert.setNull` to signify the status of any data conversion that occurred during setting.

.. py:data:: pymcsapi.CONVERT_STATUS_NONE

   There was no problems during the conversion or no conversion.

.. py:data:: pymcsapi.CONVERT_STATUS_SATURATED

   The value was saturated during the conversion, the maximum/minimum was used instead.

.. py:data:: pymcsapi.CONVERT_STATUS_INVALID

   The value was invalid during the conversion, 0 or empty string was used instead.

.. py:data:: pymcapi.CONVERT_STATUS_TRUNCATED

   The value was truncated.

columnstore_data_type constants
===============================

   These are the constants for the data types as returned by :py:meth:`ColumnStoreSystemCatalogColumn.getType`.

.. py:data:: pymcsapi.DATA_TYPE_BIT

   BIT data type

.. py:data:: pymcsapi.DATA_TYPE_TINYINT

   TINYINT data type

.. py:data:: pymcsapi.DATA_TYPE_CHAR

   CHAR data type

.. py:data:: pymcsapi.DATA_TYPE_SMALLINT

   SMALLINT data type

.. py:data:: pymcsapi.DATA_TYPE_DECIMAL

   DECIMAL data type

.. py:data:: pymcsapi.DATA_TYPE_MEDINT

   MEDIUMINT data type

.. py:data:: pymcsapi.DATA_TYPE_INT

   INT data type

.. py:data:: pymcsapi.DATA_TYPE_FLOAT

   FLOAT data type

.. py:data:: pymcsapi.DATA_TYPE_DATE

   DATE data type

.. py:data:: pymcsapi.DATA_TYPE_BIGINT

   BIGINT data type

.. py:data:: pymcsapi.DATA_TYPE_DOUBLE

   DOUBLE data type

.. py:data:: pymcsapi.DATA_TYPE_DATETIME

   DATETIME data type

.. py:data:: pymcsapi.DATA_TYPE_VARCHAR

   VARCHAR data type

.. py:data:: pymcsapi.DATA_TYPE_VARBINARY

   VARBINARY data type

.. py:data:: pymcsapi.DATA_TYPE_CLOB

   Unused

.. py:data:: pymcsapi.DATA_TYPE_BLOB

   BLOB data type

.. py:data:: pymcsapi.DATA_TYPE_UTINYINT

   UNSIGNED TINYINT data type

.. py:data:: pymcsapi.DATA_TYPE_USMALLINT

   UNSIGNED SMALLINT data type

.. py:data:: pymcsapi.DATA_TYPE_UDECIMAL

   UNSIGNED DECIMAL data type

.. py:data:: pymcsapi.DATA_TYPE_UMEDINT

   UNSIGNED MEDIUMINT data type

.. py:data:: pymcsapi.DATA_TYPE_UINT

   UNSIGNED INT data type

.. py:data:: pymcsapi.DATA_TYPE_UFLOAT

   UNSIGNED FLOAT data type

.. py:data:: pymcsapi.DATA_TYPE_UBIGINT

   UNSIGNED BIGINT data type

.. py:data:: pymcsapi.DATA_TYPE_UDOUBLE

   UNSIGNED DOUBLE data type

.. py:data:: pymcsapi.DATA_TYPE_TEXT

   TEXT data type


ColumnStoreDateTime Class
=========================

.. py:class:: ColumnStoreDateTime

   A class which is used to contain a date/time used to set ``DATE`` or ``DATETIME`` columns using :py:meth:`ColumnStoreBulkInsert.setColumn`

ColumnStoreDateTime()
---------------------

.. py:method:: pymcsapi.ColumnStoreDateTime()

   Sets the date/time to ``0000-00-00 00:00:00``.

.. py:method:: pymcsapi.ColumnStoreDateTime(time)

   Sets the date/time the value of the input string.

   :param time: The date/time to set
   :raises RuntimeError: When an invalid date or time is supplied

.. py:method:: pymcsapi.ColumnStoreDateTime(dateTime, format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :raises RuntimeError: When an invalid date or time is supplied

.. py:method:: pymcsapi.ColumnStoreDateTime(year, month, day, hour, minute, second, microsecond)

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
   :raises RuntimeError: When an invalid date or time is supplied

set()
-----

.. py:method:: ColumnStoreDateTime.set(time)

   Sets the date/time using the value of the input string.

   :param time: The date/time to set
   :returns: ``True`` if the date/time is valid, ``False`` if it is not

.. py:method:: ColumnStoreDateTime.set(dateTime, format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :returns: ``True`` if the date/time is valid, ``False`` if it is not

ColumnStoreDecimal Class
========================

.. py:class:: ColumnStoreDecimal

   A class which is used to contain a non-lossy decimal format used to set ``DECIMAL`` columns using :py:meth:`ColumnStoreBulkInsert.setColumn`.

ColumnStoreDecimal()
--------------------

.. py:method:: pymcsapi.ColumnStoreDecimal()

   Sets the decimal to ``0``.

.. py:method:: pymcsapi.ColumnStoreDecimal(value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set
   :raises RuntimeError: When an invalid value is supplied

.. py:method:: pymcsapi.ColumnStoreDecimal(value)

   Sets the decimal to the contents of a supplied string value (such as ``"3.14159"``).

   :param value: The value to set
   :raises RuntimeError: When an invalid value is supplied

.. py:method:: pymcsapi::ColumnStoreDecimal(value)

   Sets the decimal to the contents of a supplied double value.

   .. note::
      In the internally this uses the string method so the performance may be lower than expected.

   :param value: The value to set
   :raises RuntimeError: When an invalid value is supplied

.. py:method:: pymcsapi.ColumnStoreDecimal(number, scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number
   :raises RuntimeError: When an invalid number/scale is supplied

set()
-----

.. py:method:: ColumnStoreDecimal::set(value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set
   :returns: Always returns ``True``

.. py:method:: ColumnStoreDecimal.set(value)

   Sets the decimal to the contents of a supplied string value (such as ``"3.14159"``).

   :param value: The value to set
   :returns: ``True`` if the conversion was successful or ``False`` if it failed

.. py:method:: ColumnStoreDecimal.set(value)

   Sets the decimal to the contents of a supplied double value.

   .. note::
      In the internally this uses the string method so the performance may be lower than expected.

   :param value: The value to set
   :returns: ``True`` if the conversion was successful or ``False`` if it failed

.. py:method:: ColumnStoreDecimal.set(number, scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number
   :returns: ``True`` if the conversion was successful or ``False`` if it failed

ColumnStoreSystemCatalog Class
==============================

.. py:class:: ColumnStoreSystemCatalog

   A class which contains the ColumnStore system catalog of tables and columns. It should be instantiated using :py:meth:`ColumnStoreDriver.getSystemCatalog`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.

getTable()
----------

.. py:method:: ColumnStoreSystemCatalog.getTable(schemaName, tableName)

   Gets the table information for a specific table.

   :param schemaName: The schema the table is in
   :param tableName: The name of the table
   :returns: The table information
   :raises RuntimeError: If the table is not found in the system catalog

ColumnStoreSystemCatalogTable Class
===================================

.. py:class:: ColumnStoreSystemCatalogTable

   A class which contains the system catalog information for a specific table. It should be instantiated using :py:meth:`ColumnStoreSystemCatalog.getTable`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.


getSchemaName()
---------------

.. py:method:: ColumnStoreSystemCatalogTable.getSchemaName()

   Retrieves the database schema name for the table

   :returns: The schema name

getTableName()
--------------

.. py:method:: ColumnStoreSystemCatalogTable.getTableName()

   Retrieves the table name for the table

   :returns: The table name

getOID()
--------

.. py:method:: ColumnStoreSystemCatalogTable::getOID()

   Retrieves the ColumnStore object ID for the table.

   :returns: The object ID for the table

getColumnCount()
----------------

.. py:method:: ColumnStoreSystemCatalogTable.getColumnCount()

   Retrieves the number of columns in the table

   :returns: The number of columns in the table

getColumn()
-----------

.. py:method:: ColumnStoreSystemCatalogTable.getColumn(columnName)

   Retrieves the column information for a specified column by name

   :param columnName: The name of the column to retrieve
   :returns: The column information
   :raises RuntimeError: If the column is not found

.. py:method:: ColumnStoreSystemCatalogTable.getColumn(columnNumber)

   Retrieves the column information for a specified column by number starting at zero

   :param columnNumber: The number of the column to retrieve starting at ``0``
   :returns: The column information
   :raises RuntimeError: If the column is not found


ColumnStoreSystemCatalogColumn Class
====================================

.. py:class:: ColumnStoreSystemCatalogColumn

   A class containing information about a specific column in the system catalog. Should be instantiated using :py:meth:`ColumnStoreSystemCatalogTable.getColumn`.

   .. note::
      The system catalog stores schema, table and column names as lower case and therefore the functions only return lower case names. Since version 1.1.4 we make case insensitive matches.


getOID()
--------

.. py:method:: ColumnStoreSystemCatalogColumn.getOID()

   Retrieves the ColumnStore object ID for the column

   :returns: The column object ID

getColumnName()
---------------

.. py:method:: ColumnStoreSystemCatalogColumn.getColumnName()

   Retrieves the name of the column

   :returns: The column name

getDictionaryOID()
------------------

.. py:method:: ColumnStoreSystemCatalogColumn.getDictionaryOID()

   Retrieves the dictionary object ID for the column (or ``0`` if there is no dictionary)

   :returns: The dictionary object ID or ``0`` for no dictionary

getType()
---------

.. py:method:: ColumnStoreSystemCatalogColumn.getType()

   Retrieves the data type for the column

   :returns: The data type for the column

getWidth()
----------

.. py:method:: ColumnStoreSystemCatalogColumn.getWidth()

   Retrieves the width in bytes for the column

   :returns: The width in bytes

getPosition()
-------------

.. py:method:: ColumnStoreSystemCatalogColumn.getPosition()

   Retrieves the column's position in the table. The sequence of columns in the table is sorted on object ID, columns may be out-of-order if an ALTER TABLE has inserted one in the middle of the table.

   :returns: The column's position in the table

getDefaultValue()
-----------------

.. py:method:: ColumnStoreSystemCatalogColumn.getDefaultValue()

   Retrieves the default value for the column in text. The value is empty for no default.

   :returns: The column's default value

isAutoincrement()
-----------------

.. py:method:: ColumnStoreSystemCatalogColumn.isAutoincrement()

   Retrieves whether or not this column is an autoincrement column.

   :returns: ``true`` if this column is autoincrement, ``false`` if it isn't

getPrecision()
--------------

.. py:method:: ColumnStoreSystemCatalogColumn.getPrecision()

   Retrieves the decimal precision for the column.

   :returns: The decimal precision

getScale()
----------

.. py:method:: ColumnStoreSystemCatalogColumn.getScale()

   Retrieves the decimal scale for the column.

   :returns: The decimal scale

isNullable()
------------

.. py:method:: ColumnStoreSystemCatalogColumn.isNullable()

   Retrieves whether or not the column can be set to ``NULL``

   :returns: ``True`` if the column can be ``NULL`` or ``False`` if it can not

compressionType()
-----------------

.. py:method:: ColumnStoreSystemCatalogColumn.compressionType()

   Retrieves the compression type for the column. ``0`` means no compression and ``2`` means Snappy compression

   :returns: The compression type for the column
