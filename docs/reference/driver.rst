ColumnStoreDriver Class
=======================

.. envvar:: COLUMNSTORE_INSTALL_DIR

   The path to the ColumnStore installation. Used by :cpp:class:`ColumnStoreDriver`

.. cpp:namespace:: mcsapi

.. cpp:class:: ColumnStoreDriver

   This is the parent class for mcsapi. It uses the ``Columnstore.xml`` file to discover the layout of the ColumnStore cluster. It therefore needs to be able to discover the path to the ColumnStore installation.

ColumnStoreDriver()
-------------------

.. cpp:function:: ColumnStoreDriver()

   Creates an instance of the ColumnStoreDriver. This will search for the environment variable :envvar:`COLUMNSTORE_INSTALL_DIR`, if this isn't found then the default path of ``/usr/local/mariadb/columnstore/`` is used.

   :raises ColumnStoreDriverException: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libcolumnstore1/mcsapi.h>

   int main(void)
   {
       ColumnStoreDriver* driver = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver();
       } catch (mcsapi::ColumnStoreException &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete driver;
       return 0;
   }

.. cpp:function:: ColumnStoreDriver(std::string& path)

   Creates an instance of :cpp:class:`ColumnStoreDriver` using the specified path to the Columnstore.xml file (including filename).

   :param path: The path to the Columnstore.xml (including filename)
   :raises ColumnStoreDriverException: When the Columnstore.xml file cannot be found or cannot be parsed

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libcolumnstore1/mcsapi.h>

   int main(void)
   {
       ColumnStoreDriver* driver = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver("/usr/local/mariadb/columnstore/etc/Columnstore.xml");
       } catch (mcsapi::ColumnStoreException &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete driver;
       return 0;
   }

createBulkInsert()
------------------

.. cpp:function:: ColumnStoreBulkInsert* ColumnStoreDriver::createBulkInsert(std::string& db, std::string& table, uint8_t mode, uint16_t pm)

   Allocates and configures an instance of :cpp:class:`ColumnStoreBulkInsert` to be used for bulk inserts with the ColumnStore installation reference by the driver. The resulting object should be freed by the application using the library.

   :param db: The database name for the table to insert into
   :param table: The tabe name to insert into
   :param mode: Future use, must be set to ``0``
   :param pm: Future use, must be set to ``0``
   :returns: An instance of :cpp:class:`ColumnStoreBulkInsert`

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libcolumnstore1/mcsapi.h>

   int main(void)
   {
       std::string table("t1");
       std::string db("test");
       ColumnStoreDriver* driver = nullptr;
       ColumnStoreBulkInsert* bulkInsert = nullptr;
       try {
           driver = new mcsapi::ColumnStoreDriver();
           bulkInsert = driver->createBulkInsert(db, table, 0, 0);
       } catch (mcsapi::ColumnStoreException &e) {
           std::cout << "Error caught " << e.what() << std::endl;
       }
       delete bulkInsert;
       delete driver;
       return 0;
   }

getVersion()
------------

.. cpp:function:: const char* getVersion()

   Returns the version of the library in the format ``1.0.0-0393456-dirty`` where ``1.0.0`` is the version number, ``0393456`` is the short git tag and ``dirty`` signifies there is uncommitted code making up this build.

   :returns: The version string

Example
^^^^^^^
.. code-block:: cpp
   :linenos:

   #include <iostream>
   #include <libcolumnstore1/mcsapi.h>

   int main(void)
   {
       try {
           mcsapi::ColumnStoreDriver* driver = new mcsapi::ColumnStoreDriver();
           ASSERT_STREQ(GIT_VERSION, driver->getVersion());
       } catch (mcsapi::ColumnStoreException &e) {
           FAIL() << "Error caught: " << e.what() << std::endl;
       }
       return 0;
   }