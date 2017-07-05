ColumnStoreSummary Class
========================

.. cpp:namespace:: mcsapi

.. cpp:class:: ColumnStoreSummary

   A class containing the summary information for a transaction. An instance of this should be obtained from :cpp:func:`ColumnStoreBulkInsert::getSummary`.

getExecutionTime()
------------------

.. cpp:function:: double ColumnStoreSummary::getExecutionTime()

   Returns the total time for the transaction in seconds, from creation of the :cpp:class:`ColumnStoreBulkInsert` class until commit or rollback.

   :returns: The total execution time in seconds

getRowsInsertedCount()
----------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getRowsInsertedCount()

   Returns the number of rows inserted during the transaction or failed to insert for a rollback.

   :returns: The total number of rows

getTruncationCount()
--------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getTruncationCount()

   Returns the number of truncated CHAR/VARCHAR values during the transaction.

   :returns: The total number of truncated values

getSaturatedCount()
-------------------

.. cpp:function:: uint64_t ColumnStoreSummary::getSaturatedCount()

   Returns the number of saturated values during the transaction.

   :returns: The total number of saturated values

getInvalidCount()
-----------------

.. cpp:function:: uint64_t ColumnStoreSummary::getInvalidCount()

   Returns the number of invalid values during the transaction.

   .. note::
      An invalid value is one where a data conversion during :cpp:`ColumnStoreBulkInsert::setValue` was not possible. When this happens a ``0`` or empty string is used instead and the status value set accordingly.

   :returns: The total number of invalid values
