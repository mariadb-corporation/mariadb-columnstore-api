Basic Bulk Insert
=================

In this example we will insert 1000 rows of two integer values into table ``test.t1``. The full code for this can be found in the ``example/Basic_bulk_insert.java`` file in the mcsapi codebase.

You will need the following table in the test database to execute this:

.. literalinclude:: ../../example/basic_bulk_insert.sql
   :linenos:
   :language: mysql
   :caption: example/basic_bulk_insert.sql

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java 
   :lines: 23-23
   :lineno-start: 23
   :caption: example/Basic_bulk_insert.java

We need to import all classes from the package :java:package:`com.mariadb.columnstore.api` to use mcsapi.

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java
   :lines: 25-29
   :lineno-start: 25
   :caption: example/Basic_bulk_insert.java

A new instance of :java:type:`ColumnStoreDriver` is created which will attempt to find the ``Columnstore.xml`` configuration file by first searching for the environment variable :envvar:`COLUMNSTORE_INSTALL_DIR` and then the default path of ``/usr/local/mariadb/columnstore/etc/Columnstore.xml``. Alternatively we could provide a path as a parameter to :java:type:`ColumnStoreDriver`.

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java
   :lines: 30-30
   :lineno-start: 30
   :caption: example/Basic_bulk_insert.java

Once we have the ColumnStore installation's configuration in the driver we use this to initiate a bulk insert using :java:meth:`ColumnStoreDriver.createBulkInsert`. We are using the ``test`` database and the ``t1`` table. The remaining two parameters are unused for now and set to ``0``.

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java
   :lines: 31-35
   :lineno-start: 31
   :caption: example/Basic_bulk_insert.java

A "for" loop is used to loop over 1000 arbitrary inserts in this example. We use :java:meth:`ColumnStoreBulkInsert.setColumn` to specify that column ``0`` (column ``a``) should be set to the integer from the "for" loop and column ``1`` (column ``b``) is set to ``1000`` minus the integer from the "for" loop.

When we have added something to every column :java:meth:`ColumnStoreBulkInsert.writeRow` is used to indicate we are finished with the row. The library won't necessarily write the row at this stage, it buffers up to 100,000 rows by default.

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java
   :lines: 36-37
   :lineno-start: 36
   :caption: example/Basic_bulk_insert.java

At the end of the loop we execute :java:meth:`ColumnStoreBulkInsert.commit` which will send any final rows and initiate the commit of the data. If we do not do this the transaction will be implicitly rolled back instead.

.. literalinclude:: ../../example/Basic_bulk_insert.java
   :linenos:
   :language: java
   :lines: 38-41
   :lineno-start: 38
   :caption: example/Basic_bulk_insert.java

If anything fails then we should catch :java:type:`ColumnStoreException` to handle it.
