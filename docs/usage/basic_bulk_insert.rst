Basic Bulk Insert
=================

In this example we will insert 1000 rows of two integer values into table ``test.t1``. The full code for this can be found in the ``example/basic_bulk_insert.cpp`` file in the mcsapi codebase.

You will need the following table in the test database to execute this:

.. literalinclude:: ../../../example/basic_bulk_insert.sql
   :linenos:
   :caption: example/basic_bulk_insert.sql

.. literalinclude:: ../../../example/basic_bulk_insert.cpp
   :linenos:
   :lines: 25-26
   :lineno-start: 25
   :caption: example/basic_bulk_insert.cpp

We need to include ``libcolumnstore1/mcsapi.h`` which is the main include file for mcsapi. This will pull in all the required includes to use mcsapi.

.. literalinclude:: ../../../example/basic_bulk_insert.cpp
   :linenos:
   :lines: 28-31
   :lineno-start: 28
   :caption: example/basic_bulk_insert.cpp

A pointer is needed to :cpp:class:`ColumnStoreDriver` to get the configuration information about the ColumnStore installation and one to :cpp:class:`ColumnStoreBulk` to hold the class for the bulk insert.

.. literalinclude:: ../../../example/basic_bulk_insert.cpp
   :linenos:
   :lines: 32-34
   :lineno-start: 32
   :caption: example/basic_bulk_insert.cpp


A new instance of :cpp:class:`ColumnStoreDriver` is created which will attempt to find the ``Columnstore.xml`` configuration file by first searching for the environment variable :envvar:`COLUMNSTORE_INSTALL_DIR` and then the default path of ``/usr/local/mariadb/columnstore/etc/Columnstore.xml``. Alternatively we could provide a path as a parameter to :cpp:class:`ColumnStoreDriver`.

Once we have the ColumnStore installation's configuration in the driver we use this to initiate a bulk insert using :cpp:func:`ColumnStoreDriver::createBulkInsert`. We are using the ``test`` database and the ``t1`` table. The remaining two parameters are unused for now and set to ``0``.

.. literalinclude:: ../../../example/basic_bulk_insert.cpp
   :linenos:
   :lines: 35-41
   :lineno-start: 35
   :caption: example/basic_bulk_insert.cpp

A "for" loop is used to loop over 1000 arbitrary inserts in this example. We use :cpp:func:`ColumnStoreBulkInsert::setColumn` to specify that column ``0`` (column ``a``) should be set to the integer from the "for" loop and column ``1`` (column ``b``) is set to ``1000`` minus the integer from the "for" loop.

When we have added something to every column :cpp:func:`ColumnStoreBulkInsert::writeRow` is used to indicate we are finished with the row. The library won't necessarily write the row at this stage, it buffers up to 100,000 rows by default.

At the end of the loop we execute :cpp:func:`ColumnStoreBulkInsert::commit` which will send any final rows and initiate the commit of the data. If we do not do this the transaction will be implicitly rolled back instead.

.. literalinclude:: ../../../example/basic_bulk_insert.cpp
   :linenos:
   :lines: 42-47
   :lineno-start: 42
   :caption: example/basic_bulk_insert.cpp

If anything fails then we should catch :cpp:class:`ColumnStoreException` to handle it. Finally we should delete our bulk insert class and driver class instances.
