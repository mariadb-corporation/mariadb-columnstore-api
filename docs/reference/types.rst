columnstore_data_convert_status_t Type
======================================

.. cpp:namespace:: mcsapi

.. cpp:enum:: columnstore_data_convert_status_t

   The status value used in :cpp:func:`ColumnStoreBulkInsert::setColumn` and :cpp:func:`ColumnStoreBulkInsert::setNull` to signify the status of any data conversion that occurred during setting.

.. cpp:enumerator:: CONVERT_STATUS_NONE

   There was no problems during the conversion or no conversion.

.. cpp:enumerator:: CONVERT_STATUS_SATURATED

   The value was saturated during the conversion, the maximum/minimum was used instead.

.. cpp:enumerator:: CONVERT_STATUS_INVALID

   The value was invalid during the conversion, 0 or empty string was used instead.

.. cpp:enumerator:: CONVERT_STATUS_TRUNCATED

   The value was truncated.

ColumnStoreDateTime Class
=========================

.. cpp:namespace:: mcsapi

.. cpp:class:: ColumnStoreDateTime

   A class which is used to contain a date/time used to set ``DATE`` or ``DATETIME`` columns using :cpp:func:`ColumnStoreBulkInsert::setColumn`

ColumnStoreDateTime()
---------------------

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime()

   Sets the date/time to ``0000-00-00 00:00:00``.

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime(tm& time)

   Sets the date/time the value of the :cpp:type`tm` struct.

   :param time: The date/time to set
   :raises ColumnStoreException: When an invalid date or time is supplied

.. cpp:function:: ColumnStoreDateTime::ColumnStoreDateTime(std::string& dateTime, std::string& format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :raises ColumnStoreException: When an invalid date or time is supplied

set()
-----

.. cpp:function:: ColumnStoreDateTime::bool set(tm& time)

   Sets the date/time using the value of the :cpp:type:`tm` struct.

   :param time: The date/time to set
   :returns: ``true`` if the date/time is valid, ``false`` if it is not

.. cpp:function:: ColumnStoreDateTime::bool set(std::string& dateTime, std::string& format)

   Sets the date/time based on a given string and format.

   :param dateTime: A string containing the date/time to set
   :param format: The format specifier for the date/time string. This uses the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.
   :returns: ``true`` if the date/time is valid, ``false`` if it is not

ColumnStoreDecimal Class
========================

.. cpp:namespace:: mcsapi

.. cpp:class:: ColumnStoreDecimal

   A class which is used to contain a non-lossy decimal format used to set ``DECIMAL`` columns using :cpp:func:`ColumnStoreBulkInsert::setColumn`.

ColumnStoreDecimal()
--------------------

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal()

   Sets the decimal to ``0``.

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(int64_t value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(std::string& value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   :param value: The value to set

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(double value)

   Sets the decimal to the contents of a supplied :cpp:type:`double` value.

   .. note::
      The internally this uses the :cpp:type:`std::string` method so the performance may be lower than expected.

   :param value: The value to set

.. cpp:function:: ColumnStoreDecimal::ColumnStoreDecimal(int64_t number, uint8_t scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number

set()
-----

.. cpp:function:: ColumnStoreDecimal::bool set(int64_t value)

   Sets the decimal to an supplied integer value.

   :param value: The value to set
   :returns: Always returns ``true``

.. cpp:function:: ColumnStoreDecimal::bool set(std::string& value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   :param value: The value to set
   :returns: ``true`` if the conversion was successful or ``false`` if it failed

.. cpp:function:: ColumnStoreDecimal::bool set(double value)

   Sets the decimal to the contents of a supplied :cpp:type:`std::string` value (such as ``"3.14159"``).

   .. note::
      The internally this uses the :cpp:type:`std::string` method so the performance may be lower than expected.

   :param value: The value to set
   :returns: ``true`` if the conversion was successful or ``false`` if it failed

.. cpp:function:: ColumnStoreDecimal::bool set(int64_t number, uint8_t scale)

   Sets the decimal to a given number and scale. For example for the value 3.14159 you would set the number to ``314159`` and the scale to ``5``.

   :param number: The number to set
   :param scale: The scale for the number
   :returns: ``true`` if the conversion was successful or ``false`` if it failed
