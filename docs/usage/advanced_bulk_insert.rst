Advanced Bulk Insert
====================

In this example we will insert 2 rows in a more complex table. This will demonstrate using different kinds of data types, chanined methods and getting the summary information at the end of a transaction.

You will need the following table in the test database to execute this:

.. literalinclude:: ../../example/advanced_bulk_insert.sql
   :linenos:
   :language: mysql
   :caption: example/advanced_bulk_insert.sql

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 25-34
   :lineno-start: 25
   :caption: example/advanced_bulk_insert.cpp

As with the basic example we create an instance of the driver and use it to create a bulk insert instance.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 35-40
   :lineno-start: 35
   :caption: example/advanced_bulk_insert.cpp

This demonstrates setting several different data types using strings of data. The second column (column ``1``) is a ``VARCHAR(40)`` and is set to "Andrew". The third column is a ``DATE`` column and the API will automatically convert this into a binary date format before transmitting it to ColumnStore. The fourth is a ``DATETIME`` and the fifth a ``DECIMAL`` column which again the API will convert from the strings into the binary format.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 41-44
   :lineno-start: 41
   :caption: example/advanced_bulk_insert.cpp

:cpp:class:`ColumnStoreDateTime` is used to create an entry for a ``DATE`` or ``DATETIME`` column. It can be used to define custom formats for dates and times using the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 45-46
   :lineno-start: 45
   :caption: example/advanced_bulk_insert.cpp

A decimal is created using the :cpp:class:`ColumnStoreDecimal` class. It can be set using a string, :c:type:`double` or a pair of integers. The first integer is the precision and the second integer is the scale. So this number becomes ``23476.23``.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 47-49
   :lineno-start: 47
   :caption: example/advanced_bulk_insert.cpp

Many of the :cpp:class:`ColumnStoreBulkInsert` methods return a pointer to the class which means multiple calls can be chained together in a similar way to ORM APIs. Here we can also see the dates and decimal we set earlier are applied.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 50-52
   :lineno-start: 50
   :caption: example/advanced_bulk_insert.cpp

After a commit or rollback we can obtain summary information from the bulk insert class. This is done using the :cpp:func:`ColumnStoreBulkInsert::getSummary` method which will return a reference :cpp:class:`ColumnStoreSummary` class. In this example we get the number of rows inserted (or would be inserted if there was a rollback) and the execution time from the moment the bulk insert class is created until the commit or rollback is complete.

.. literalinclude:: ../../example/advanced_bulk_insert.cpp
   :linenos:
   :lines: 53-58
   :lineno-start: 53
   :caption: example/advanced_bulk_insert.cpp

At the end we clean up in the same was as the basic bulk insert example.
