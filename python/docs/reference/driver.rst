pymcsapi Module
===============

.. py:module pymcsapi

   This is the main wrapper module that invoces functions of the base C++ library mcsapi.


ColumnStoreDriver Class
=======================

.. py:class:: ColumnStoreDriver

   This is the parent class for pymcsapi. It uses the ``Columnstore.xml`` file to discover the layout of the ColumnStore cluster. It therefore needs to be able to discover the path to the ColumnStore installation.

ColumnStoreDriver()
-------------------

.. py:method:: pymcsapi.ColumnStoreDriver()

   Creates an instance of the ColumnStoreDriver. This will search for the environment variable :envvar:`COLUMNSTORE_INSTALL_DIR`, if this isn't found then the default path of ``/usr/local/mariadb/columnstore/`` is used.

   :raises RuntimeError: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: python
   :linenos:
   
   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
   except RuntimeError as err:
       print("Error caught: %s" % (err,)) 

.. py:method:: pymcsapi.ColumnStoreDriver(path)

   Creates an instance of ``ColumnStoreDriver`` using the specified path to the Columnstore.xml file (including filename).

   :param path: The path to the Columnstore.xml (including filename)
   :raises RuntimeError: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver('/usr/local/mariadb/columnstore/etc/Columnstore.xml')
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

createBulkInsert()
------------------

.. py:method:: ColumnStoreDriver.createBulkInsert(db, table, mode, pm)

   Allocates and configures an instance of :py:class:`ColumnStoreBulkInsert` to be used for bulk inserts with the ColumnStore installation reference by the driver. 

   :param db: The database name for the table to insert into
   :param table: The tabe name to insert into
   :param mode: Future use, must be set to ``0``
   :param pm: Future use, must be set to ``0``. For now batches of inserts use a round-robin between the PM servers.
   :returns: An instance of :py:class:`ColumnStoreBulkInsert`
   :raises RuntimeError: If a table lock cannot be acquired for the desired table

Example
^^^^^^^
.. code-block:: python
   :linenos:
 
   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       bulkInsert = driver.createBulkInsert("test", "t1", 0, 0);
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

getVersion()
------------

.. py:method:: ColumnStoreDriver.getVersion()

   Returns the version of the mcsapi library in the format ``1.0.0-0393456-dirty`` where ``1.0.0`` is the version number, ``0393456`` is the short git tag and ``dirty`` signifies there is uncommitted code making up this build.

   :returns: The mcsapi version string

Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       print("mcsapi version: %s" % (driver.getVersion(),))
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

setDebug()
----------

.. py:method:: ColumnStoreDriver.setDebug(enabled)

   Enables/disables verbose debugging output which is sent to stderr upon execution.

   .. note::
      This is a global setting which will apply to all instances of all of the API's classes after it is set until it is turned off.

   :param enabled: Set to ``True`` to enable and ``False`` to disable.

Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       driver.setDebug(True)
       # Debugging output is now enabled
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

getSystemCatalog()
------------------

.. py:method:: ColumnStoreDriver.getSystemCatalog()

   Returns an instance of the ColumnStore system catalog which contains all of the ColumnStore table and column details

   :returns: The system catalog

Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       sysCat = driver.getSystemCatalog()
       table = sysCat.getTable("test", "t1")
       print("t1 has %d columns" % (table.getColumnCount(),))
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

