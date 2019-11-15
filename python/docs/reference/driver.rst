ColumnStoreDriver Class
=======================

.. py:class:: ColumnStoreDriver

   This is the parent class for pymcsapi. It uses the ``Columnstore.xml`` file to discover the layout of the ColumnStore cluster. It therefore needs to be able to discover the path to the ColumnStore installation.

ColumnStoreDriver()
-------------------

.. py:method:: pymcsapi.ColumnStoreDriver()

   Creates an instance of the ColumnStoreDriver. The default path of ``/etc/columnstore/`` is used to find the configuration file.

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
       driver = pymcsapi.ColumnStoreDriver('/etc/columnstore/Columnstore.xml')
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

.. py:method:: ColumnStoreDriver.setDebug(level)

   Enables/disables verbose debugging output which is sent to stderr upon execution. Levels are as follows:
   
   * ``0`` - Off
   * ``1`` - Show messages and binary packets truncated at 1000 bytes
   * ``2`` - Show full messages, full length binary packets and ASCII translations

   .. note::
      This is a global setting which will apply to all instances of all of the API's classes after it is set until it is turned off.

   :param level: Set to the log level required, ``0`` = off.

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

       
listTableLocks()
----------------

.. py:method:: ColumnStoreDriver.listTableLocks()

   Returns a tuple of TableLockInfo objects that contains information about the current table locks in the ColumnStore system.
   
   :returns: A tuple of TableLockInfo objects
   
Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       tliv = driver.listTableLocks()
   except RuntimeError as err:
       print("Error caught: %s" % (err,))
    
    
isTableLocked()
---------------

.. py:method:: ColumnStoreDriver.isTableLocked(db, table)

   Returns ``True`` if the specified table is locked and ``False`` if it is not locked.
   
   :param db: The database name for the table to check
   :param table: The tabe name to check
   :returns: ``True`` if the specified table is locked, otherwise ``False``
   :raises RuntimeError: If the specified table is not existent
   
Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       locked = driver.isTableLocked("test","tmp1")
   except RuntimeError as err:
       print("Error caught: %s" % (err,))
       
.. note::
   Only locks of tables that have been existent when ColumnStoreDriver was created can be detected.
   
   
clearTableLock()
----------------

.. py:method:: ColumnStoreDriver.clearTableLock(lockId)

   Clears a table lock with given id
   
   :param lockId: The id of the table lock to clear
   
Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       tliv = driver.listTableLocks()
       for tli in tliv:
           driver.clearTableLock(tli.id)
   except RuntimeError as err:
       print("Error caught: %s" % (err,))


.. py:method:: ColumnStoreDriver.clearTableLock(tableLockInfo)

   Clears a table lock with given TableLockInfo element using its lock id
   
   :param lockId: The TableLockInfo object whose id will be used to clear the lock
   
Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       tliv = driver.listTableLocks()
       for tli in tliv:
           driver.clearTableLock(tli)
   except RuntimeError as err:
       print("Error caught: %s" % (err,))
       

.. py:method:: ColumnStoreDriver.clearTableLock(db, table)

   Clears a table lock of given database table combination
   
   :param db: The database name for the table to clear
   :param table: The tabe name to clear
   
Example
^^^^^^^
.. code-block:: python
   :linenos:

   import pymcsapi

   try:
       driver = pymcsapi.ColumnStoreDriver()
       driver.clearTableLock("test","tmp1")
   except RuntimeError as err:
       print("Error caught: %s" % (err,))

