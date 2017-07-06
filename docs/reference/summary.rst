ColumnStoreSummary Class
========================

.. cpp:class:: ColumnStoreSummary

   A class containing the summary information for a transaction. An instance of this should be obtained from :cpp:func:`ColumnStoreBulkInsert::getSummary`.

getExecutionTime()
------------------

.. cpp:function:: double ColumnStoreSummary::getExecutionTime()

   Returns the total time for the transaction in seconds, from creation of the :cpp:class:`ColumnStoreBulkInsert` class until commit or rollback.

   :returns: The total execution time in seconds

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // Get the summary, note that we don't free this
   ColumnStoreSummary* summary = bulkInsert->getSummary();

   // Get the execution time for the transaction
   double execTime = summary->getExecutionTime();
   ...

getRowsInsertedCount()
----------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getRowsInsertedCount()

   Returns the number of rows inserted during the transaction or failed to insert for a rollback.

   :returns: The total number of rows

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   bulkInsert->setValue(1, (uint32_t) 654321);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // Get the summary, note that we don't free this
   ColumnStoreSummary* summary = bulkInsert->getSummary();

   // Get the number of inserted rows before they were rolled back
   uint64_t rows = summary->getRowsInsertedCount();
   ...

getTruncationCount()
--------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getTruncationCount()

   Returns the number of truncated CHAR/VARCHAR values during the transaction.

   :returns: The total number of truncated values

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   std::string strVal("Short string");

   // A short string that will insert fine
   bulkInsert->setValue(0, strVal);

   // This long string will truncate on my VARCHAR(20)
   strVal = "This is a long string test to demonstrate a truncation";
   bulkInsert->setValue(1, strVal);

   // Get the number of truncated values before they were rolled back
   uint64_t truncateCount = summary->getTruncationCount();
   ...

getSaturatedCount()
-------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getSaturatedCount()

   Returns the number of saturated values during the transaction.

   :returns: The total number of saturated values

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   // Slightly higher than a signed int max, this will saturate
   bulkInsert->setValue(1, (uint32_t) 2147483650);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // Get the summary, note that we don't free this
   ColumnStoreSummary* summary = bulkInsert->getSummary();

   // Get the number of saturated values before they were rolled back
   uint64_t saturatedCount = summary->getSaturatedCount();
   ...


getInvalidCount()
-----------------

.. cpp:function:: uint64_t ColumnStoreSummary::getInvalidCount()

   Returns the number of invalid values during the transaction.

   .. note::
      An invalid value is one where a data conversion during :cpp:func:`ColumnStoreBulkInsert::setValue` was not possible. When this happens a ``0`` or empty string is used instead and the status value set accordingly.

   :returns: The total number of invalid values

Example
^^^^^^^

This example can be used inside the try...catch blocks in the :cpp:class:`ColumnStoreDriver` examples.

.. code-block:: cpp
   :linenos:

   ...
   driver = new mcsapi::ColumnStoreDriver();
   bulkInsert = driver->createBulkInsert(db, table, 0, 0);

   // Set values for a 2 int column table
   bulkInsert->setValue(0, (uint32_t) 123456);
   // This is a DATE column, which is invalid to set as a date.
   // The result will be the date set to '0000-00-00'
   // and a invalid counter increment
   bulkInsert->setValue(1, (uint32_t) 123456);

   // Write the row
   bulkInsert->writeRow();

   // Rollback the transaction
   bulkInsert->rollback();

   // Get the summary, note that we don't free this
   ColumnStoreSummary* summary = bulkInsert->getSummary();

   // Get the number of invalid values before they were rolled back
   uint64_t invalidCount = summary->getInvalidCount();
   ...
