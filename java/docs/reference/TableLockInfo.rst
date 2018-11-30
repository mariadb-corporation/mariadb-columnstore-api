TableLockInfo
=============

.. java:package:: com.mariadb.columnstore.api
   :noindex:

.. java:type:: public class TableLockInfo

Fields
------
swigCMemOwn
^^^^^^^^^^^

.. java:field:: protected transient boolean swigCMemOwn
   :outertype: TableLockInfo

Constructors
------------
TableLockInfo
^^^^^^^^^^^^^

.. java:constructor:: protected TableLockInfo(long cPtr, boolean cMemoryOwn)
   :outertype: TableLockInfo

TableLockInfo
^^^^^^^^^^^^^

.. java:constructor:: public TableLockInfo() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

Methods
-------
delete
^^^^^^

.. java:method:: public synchronized void delete() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

finalize
^^^^^^^^

.. java:method:: protected void finalize()
   :outertype: TableLockInfo

getCPtr
^^^^^^^

.. java:method:: protected static long getCPtr(TableLockInfo obj)
   :outertype: TableLockInfo

getCreationTime
^^^^^^^^^^^^^^^

.. java:method:: public SWIGTYPE_p_time_t getCreationTime() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getDbrootList
^^^^^^^^^^^^^

.. java:method:: public SWIGTYPE_p_std__vectorT_unsigned_int_t getDbrootList() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getId
^^^^^

.. java:method:: public java.math.BigInteger getId() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getOwnerName
^^^^^^^^^^^^

.. java:method:: public String getOwnerName() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getOwnerPID
^^^^^^^^^^^

.. java:method:: public long getOwnerPID() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getOwnerSessionID
^^^^^^^^^^^^^^^^^

.. java:method:: public long getOwnerSessionID() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getOwnerTxnID
^^^^^^^^^^^^^

.. java:method:: public long getOwnerTxnID() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getState
^^^^^^^^

.. java:method:: public columnstore_lock_types_t getState() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

getTableOID
^^^^^^^^^^^

.. java:method:: public long getTableOID() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setCreationTime
^^^^^^^^^^^^^^^

.. java:method:: public void setCreationTime(SWIGTYPE_p_time_t value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setDbrootList
^^^^^^^^^^^^^

.. java:method:: public void setDbrootList(SWIGTYPE_p_std__vectorT_unsigned_int_t value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setId
^^^^^

.. java:method:: public void setId(java.math.BigInteger value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setOwnerName
^^^^^^^^^^^^

.. java:method:: public void setOwnerName(String value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setOwnerPID
^^^^^^^^^^^

.. java:method:: public void setOwnerPID(long value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setOwnerSessionID
^^^^^^^^^^^^^^^^^

.. java:method:: public void setOwnerSessionID(long value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setOwnerTxnID
^^^^^^^^^^^^^

.. java:method:: public void setOwnerTxnID(long value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setState
^^^^^^^^

.. java:method:: public void setState(columnstore_lock_types_t value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

setTableOID
^^^^^^^^^^^

.. java:method:: public void setTableOID(long value) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: TableLockInfo

