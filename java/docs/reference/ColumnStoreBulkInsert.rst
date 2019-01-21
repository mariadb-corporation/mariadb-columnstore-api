ColumnStoreBulkInsert
=====================

.. java:package:: com.mariadb.columnstore.api
   :noindex:

.. java:type:: public class ColumnStoreBulkInsert

Fields
------
swigCMemOwn
^^^^^^^^^^^

.. java:field:: protected transient boolean swigCMemOwn
   :outertype: ColumnStoreBulkInsert

Constructors
------------
ColumnStoreBulkInsert
^^^^^^^^^^^^^^^^^^^^^

.. java:constructor:: protected ColumnStoreBulkInsert(long cPtr, boolean cMemoryOwn)
   :outertype: ColumnStoreBulkInsert

Methods
-------
addReference
^^^^^^^^^^^^

.. java:method:: protected void addReference(ColumnStoreDriver columnStoreDriver)
   :outertype: ColumnStoreBulkInsert

commit
^^^^^^

.. java:method:: public void commit() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

delete
^^^^^^

.. java:method:: public synchronized void delete() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

finalize
^^^^^^^^

.. java:method:: protected void finalize()
   :outertype: ColumnStoreBulkInsert

getCPtr
^^^^^^^

.. java:method:: protected static long getCPtr(ColumnStoreBulkInsert obj)
   :outertype: ColumnStoreBulkInsert

getColumnCount
^^^^^^^^^^^^^^

.. java:method:: public int getColumnCount() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

getSummary
^^^^^^^^^^

.. java:method:: public ColumnStoreSummary getSummary() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

isActive
^^^^^^^^

.. java:method:: public boolean isActive() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

resetRow
^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert resetRow() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

rollback
^^^^^^^^

.. java:method:: public void rollback() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setBatchSize
^^^^^^^^^^^^

.. java:method:: public void setBatchSize(long batchSize) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, String value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, String value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, String value, long length, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, String value, long length) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, java.math.BigInteger value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, java.math.BigInteger value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, long value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, long value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, int value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, int value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, short value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, short value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, byte value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, byte value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, boolean value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, boolean value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, double value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, double value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreDateTime value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreDateTime value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreTime value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreTime value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreDecimal value, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setColumn
^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setColumn(int columnNumber, ColumnStoreDecimal value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setNull
^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setNull(int columnNumber, int[] status) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setNull
^^^^^^^

.. java:method:: public ColumnStoreBulkInsert setNull(int columnNumber) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

setTruncateIsError
^^^^^^^^^^^^^^^^^^

.. java:method:: public void setTruncateIsError(boolean set) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

writeRow
^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert writeRow() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreBulkInsert

