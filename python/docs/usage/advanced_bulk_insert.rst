Advanced Bulk Insert
====================

In this example we will insert 2 rows in a more complex table. This will demonstrate using different kinds of data types, chanined methods and getting the summary information at the end of a transaction.

You will need the following table in the test database to execute this:

.. literalinclude:: ../../example/advanced_bulk_insert.sql
   :linenos:
   :language: mysql
   :caption: example/advanced_bulk_insert.sql

.. literalinclude:: ../../example/advanced_bulk_insert.py
   :linenos:
   :language: python
   :lines: 26-30
   :lineno-start: 26
   :caption: example/advanced_bulk_insert.py

As with the basic example we create an instance of the driver and use it to create a bulk insert instance.

.. literalinclude:: ../../example/advanced_bulk_insert.py
   :linenos:
   :language: python
   :lines: 31-36
   :lineno-start: 31
   :caption: example/advanced_bulk_insert.py

This demonstrates setting several different data types using strings of data. The second column (column ``1``) is a ``VARCHAR(40)`` and is set to "Andrew". The third column is a ``DATE`` column and the API will automatically convert this into a binary date format before transmitting it to ColumnStore. The fourth is a ``DATETIME`` and the fifth a ``DECIMAL`` column which again the API will convert from the strings into the binary format.

.. literalinclude:: ../../example/advanced_bulk_insert.py
   :linenos:
   :language: python
   :lines: 37-37
   :lineno-start: 37
   :caption: example/advanced_bulk_insert.py

:py:class:`ColumnStoreDateTime` is used to create an entry for a ``DATE`` or ``DATETIME`` column. It can be used to define custom formats for dates and times using the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.

A decimal is created using the :py:class:`ColumnStoreDecimal` class. It can be set using a string, :c:type:`double` or a pair of integers. The first integer is the precision and the second integer is the scale. So this number becomes ``23476.23``.

Many of the :py:class:`ColumnStoreBulkInsert` methods return a pointer to the class which means multiple calls can be chained together in a similar way to ORM APIs. Here we can also see the dates and decimal we set earlier are applied.

.. literalinclude:: ../../example/advanced_bulk_insert.py
   :linenos:
   :language: python
   :lines: 38-43
   :lineno-start: 38
   :caption: example/advanced_bulk_insert.py

After a commit or rollback we can obtain summary information from the bulk insert class. This is done using the :py:meth:`ColumnStoreBulkInsert.getSummary` method which will return a reference :py:class:`ColumnStoreSummary` class. In this example we get the number of rows inserted (or would be inserted if there was a rollback) and the execution time from the moment the bulk insert class is created until the commit or rollback is complete.

.. literalinclude:: ../../example/advanced_bulk_insert.py
   :linenos:
   :language: python
   :lines: 44-45
   :lineno-start: 44
   :caption: example/advanced_bulk_insert.py

At the end we clean up in the same was as the basic bulk insert example.
