ColumnStoreDriver Class
=======================

.. envvar:: COLUMNSTORE_INSTALL_DIR

   The optional environment variable containing the path to the ColumnStore installation. Used by :cpp:class:`ColumnStoreDriver`

.. cpp:class:: ColumnStoreDriver

   This is the parent class for mcsapi. It uses the ``Columnstore.xml`` file to discover the layout of the ColumnStore cluster. It therefore needs to be able to discover the path to the ColumnStore installation.

ColumnStoreDriver()
-------------------

.. cpp:function:: ColumnStoreDriver::ColumnStoreDriver()

   Creates an instance of the ColumnStoreDriver. This will search for the environment variable :envvar:`COLUMNSTORE_INSTALL_DIR`, if this isn't found then the default path of ``/usr/local/mariadb/columnstore/`` is used.

   :raises ColumnStoreConfigError: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       mcsapi::ColumnStoreDriver* driver = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver();
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete driver;
       return 0;
   }

.. cpp:function:: ColumnStoreDriver::ColumnStoreDriver(const std::string& path)

   Creates an instance of ``ColumnStoreDriver`` using the specified path to the Columnstore.xml file (including filename).

   :param path: The path to the Columnstore.xml (including filename)
   :raises ColumnStoreConfigError: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       mcsapi::ColumnStoreDriver* driver = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver("/usr/local/mariadb/columnstore/etc/Columnstore.xml");
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete driver;
       return 0;
   }

createBulkInsert()
------------------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreDriver::createBulkInsert(const std::string& db, const std::string& table, uint8_t mode, uint16_t pm)

   Allocates and configures an instance of :cpp:class:`ColumnStoreBulkInsert` to be used for bulk inserts with the ColumnStore installation reference by the driver. The resulting object should be freed by the application using the library.

   :param db: The database name for the table to insert into
   :param table: The tabe name to insert into
   :param mode: Future use, must be set to ``0``
   :param pm: Future use, must be set to ``0``. For now batches of inserts use a round-robin between the PM servers.
   :returns: An instance of :cpp:class:`ColumnStoreBulkInsert`
   :raises ColumnStoreServerError: If a table lock cannot be acquired for the desired table

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       std::string table("t1");
       std::string db("test");
       mcsapi::ColumnStoreDriver* driver = nullptr;
       mcsapi::ColumnStoreBulkInsert* bulkInsert = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver();
           bulkInsert = driver->createBulkInsert(db, table, 0, 0);
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete bulkInsert;
       delete driver;
       return 0;
   }

getVersion()
------------

.. cpp:function:: const char* ColumnStoreDriver::getVersion()

   Returns the version of the library in the format ``1.0.0-0393456-dirty`` where ``1.0.0`` is the version number, ``0393456`` is the short git tag and ``dirty`` signifies there is uncommitted code making up this build.

   :returns: The version string

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try {
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           const char* version = driver->getVersion();
           std::cout << version << std::endl;
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }

setDebug()
----------

.. cpp:function:: void ColumnStoreDriver::setDebug(uint8_t level)

   Enables/disables verbose debugging output which is sent to stderr upon execution. Levels are as follows:

   * ``0`` - Off
   * ``1`` - Show messages and binary packets truncated at 1000 bytes
   * ``2`` - Show full messages, full length binary packets and ASCII translations

   .. note::
      This is a global setting which will apply to all instances of all of the API's classes after it is set until it is turned off.

   :param level: Set to the log level required, ``0`` = off.

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try {
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           driver->setDebug(true);
           // Debugging output is now enabled
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


getSystemCatalog()
------------------

.. cpp:function:: ColumnStoreSystemCatalog& ColumnStoreDriver::getSystemCatalog()

   Returns an instance of the ColumnStore system catalog which contains all of the ColumnStore table and column details

   :returns: The system catalog

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try {
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           mcsapi::ColumnStoreSystemCatalog sysCat = driver->getSystemCatalog();

           mcsapi::ColumnStoreSystemCatalogTable tbl = sysCat.getTable("test", "t1");
           std::cout << "t1 has " << tbl.getColumnCount() << " columns" << endl;

           mcsapi::ColumnStoreSystemCatalogColumn col1 = tbl.getColumn(0);         
           std::cout << "The first column in t1 is " << col1.getColumnName() << endl;
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


listTableLocks()
----------------

.. cpp:function:: std::vector<mcsapi::TableLockInfo> ColumnStoreDriver::listTableLocks()

   Returns a vector of TableLockInfo objects that contains information about the current table locks in the ColumnStore system.

   :returns: A vector of mcsapi::TableLockInfo objects

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           std::vector<mcsapi::TableLockInfo> tliv = driver.listTableLocks();
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


isTableLocked()
---------------

.. cpp:function:: bool ColumnStoreDriver::isTableLocked(const std::string& db, const std::string& table)

   Returns ``true`` if the specified table is locked and ``false`` if it is not locked.

   :param db: The database name for the table to check
   :param table: The tabe name to check
   :returns: ``true`` if the specified table is locked, otherwise ``false``
   :raises ColumnStoreServerError: If the specified table is not existent

.. note::
   Only locks of tables that have been existent when ColumnStoreDriver was created can be detected.

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           bool locked = driver.isTableLocked("test","tmp1");
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


.. cpp:function:: bool ColumnStoreDriver::isTableLocked(const std::string& db, const std::string& table, TableLockInfo& rtn)

   Returns ``true`` if the specified table is locked and ``false`` if it is not locked. Further information about the table lock can be accessed through the referrenced TableLockInfo.

   :param db: The database name for the table to check
   :param table: The tabe name to check
   :param rtn: The TableLockInfo object reference to store further information about the table lock into.
   :returns: ``true`` if the specified table is locked, otherwise ``false``
   :raises ColumnStoreServerError: If the specified table is not existent

.. note::
   Only locks of tables that have been existent when ColumnStoreDriver was created can be detected.

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           mcsapi::TableLockInfo tli;
           bool locked = driver.isTableLocked("test","tmp1",tli);
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


clearTableLock()
----------------

.. cpp:function:: void ColumnStoreDriver::clearTableLock(uint64_t lockId)

   Clears a table lock with given id

   :param lockId: The id of the table lock to clear

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           std::vector<mcsapi::TableLockInfo> tliv = driver.listTableLocks();
           for( auto& tli : tliv ){
               driver.clearTableLock(tli.id);
           }
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


.. cpp:function:: void ColumnStoreDriver::clearTableLock(mcsapi::TableLockInfo tli)

   Clears a table lock with given TableLockInfo element using its lock id

   :param tli: The TableLockInfo object whose id will be used to clear the lock

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           std::vector<mcsapi::TableLockInfo> tliv = driver.listTableLocks();
           for( auto& tli : tliv ){
               driver.clearTableLock(tli);
           }
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }


.. cpp:function:: void ColumnStoreDriver::clearTableLock(const std::string & db, const std::string & table)

   Clears a table lock of given database table combinationd

   :param db: The database name for the table to clear
   :param table: The tabe name to clear

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libmcsapi/mcsapi.h>

   int main(void)
   {
       try{
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           driver.clearTableLock("test","tmp1");
       } catch (mcsapi::ColumnStoreError &e) {
           std::cout << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }

