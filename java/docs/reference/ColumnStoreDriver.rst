.. java:import:: java.util Enumeration

.. java:import:: java.net URL

.. java:import:: java.util.jar Manifest

.. java:import:: java.io IOException

ColumnStoreDriver
=================

.. java:package:: com.mariadb.columnstore.api
   :noindex:

.. java:type:: public class ColumnStoreDriver

Fields
------
swigCMemOwn
^^^^^^^^^^^

.. java:field:: protected transient boolean swigCMemOwn
   :outertype: ColumnStoreDriver

Constructors
------------
ColumnStoreDriver
^^^^^^^^^^^^^^^^^

.. java:constructor:: protected ColumnStoreDriver(long cPtr, boolean cMemoryOwn)
   :outertype: ColumnStoreDriver

ColumnStoreDriver
^^^^^^^^^^^^^^^^^

.. java:constructor:: public ColumnStoreDriver(String path) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

ColumnStoreDriver
^^^^^^^^^^^^^^^^^

.. java:constructor:: public ColumnStoreDriver() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

Methods
-------
clearTableLock
^^^^^^^^^^^^^^

.. java:method:: public void clearTableLock(java.math.BigInteger lockId) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

clearTableLock
^^^^^^^^^^^^^^

.. java:method:: public void clearTableLock(TableLockInfo tbi) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

clearTableLock
^^^^^^^^^^^^^^

.. java:method:: public void clearTableLock(String db, String table) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

createBulkInsert
^^^^^^^^^^^^^^^^

.. java:method:: public ColumnStoreBulkInsert createBulkInsert(String db, String table, short mode, int pm) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

delete
^^^^^^

.. java:method:: public synchronized void delete() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

finalize
^^^^^^^^

.. java:method:: protected void finalize()
   :outertype: ColumnStoreDriver

getCPtr
^^^^^^^

.. java:method:: protected static long getCPtr(ColumnStoreDriver obj)
   :outertype: ColumnStoreDriver

getJavaMcsapiVersion
^^^^^^^^^^^^^^^^^^^^

.. java:method:: public String getJavaMcsapiVersion()
   :outertype: ColumnStoreDriver

getSystemCatalog
^^^^^^^^^^^^^^^^

.. java:method:: public ColumnStoreSystemCatalog getSystemCatalog() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

getVersion
^^^^^^^^^^

.. java:method:: public String getVersion() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

isTableLocked
^^^^^^^^^^^^^

.. java:method:: public boolean isTableLocked(String db, String table, TableLockInfo rtn) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

isTableLocked
^^^^^^^^^^^^^

.. java:method:: public boolean isTableLocked(String db, String table) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

listTableLocks
^^^^^^^^^^^^^^

.. java:method:: public TableLockInfoVector listTableLocks() throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

setDebug
^^^^^^^^

.. java:method:: public void setDebug(short level) throws com.mariadb.columnstore.api.ColumnStoreException
   :outertype: ColumnStoreDriver

