Advanced Bulk Insert
====================

In this example we will insert 2 rows in a more complex table. This will demonstrate using different kinds of data types, chanined methods and getting the summary information at the end of a transaction.

You will need the following table in the test database to execute this:

.. literalinclude:: ../../example/advanced_bulk_insert.sql
   :linenos:
   :language: mysql
   :caption: example/advanced_bulk_insert.sql

.. literalinclude:: ../../example/Advanced_bulk_insert.java
   :linenos:
   :language: java
   :lines: 23-30
   :lineno-start: 23
   :caption: example/Advanced_bulk_insert.java

As with the basic example we create an instance of the driver and use it to create a bulk insert instance.

.. literalinclude:: ../../example/Advanced_bulk_insert.java
   :linenos:
   :language: java
   :lines: 31-36
   :lineno-start: 31
   :caption: example/Advanced_bulk_insert.java

This demonstrates setting several different data types using strings of data. The second column (column ``1``) is a ``VARCHAR(40)`` and is set to "Andrew". The third column is a ``DATE`` column and the API will automatically convert this into a binary date format before transmitting it to ColumnStore. The fourth is a ``DATETIME`` and the fifth a ``DECIMAL`` column which again the API will convert from the strings into the binary format.

.. literalinclude:: ../../example/Advanced_bulk_insert.java
   :linenos:
   :language: java
   :lines: 37-37
   :lineno-start: 37
   :caption: example/Advanced_bulk_insert.java

Many of the :java:type:`ColumnStoreBulkInsert` methods return a pointer to the class and a return status which means multiple calls can be chained together in a similar way to ORM APIs. Here we use additional datatypes :java:type:`ColumnStoreDateTime` and :java:type:`ColumnStoreDecimal`.

:java:type:`ColumnStoreDateTime` is used to create an entry for a ``DATE`` or ``DATETIME`` column. It can be used to define custom formats for dates and times using the `strptime format <http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html>`_.

A decimal is created using the :java:type:`ColumnStoreDecimal` class. It can be set using a string, :c:type:`double` or a pair of integers. The first integer is the precision and the second integer is the scale. So this number becomes ``23476.23``.

.. literalinclude:: ../../example/Advanced_bulk_insert.java
   :linenos:
   :language: java
   :lines: 38-45
   :lineno-start: 38
   :caption: example/Advanced_bulk_insert.java

After a commit or rollback we can obtain summary information from the bulk insert class. This is done using the :java:meth:`ColumnStoreBulkInsert.getSummary` method which will return a reference :java:type:`ColumnStoreSummary` class. In this example we get the number of rows inserted (or would be inserted if there was a rollback) and the execution time from the moment the bulk insert class is created until the commit or rollback is complete.

.. literalinclude:: ../../example/Advanced_bulk_insert.java
   :linenos:
   :language: java
   :lines: 46-50
   :lineno-start: 46
   :caption: example/Advanced_bulk_insert.java

At the end we clean up in the same was as the basic bulk insert example.
