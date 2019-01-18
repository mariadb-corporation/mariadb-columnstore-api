Version History
===============

This is a version history of Java API interface changes. It does not include internal fixes and changes.

+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| Version | Changes                                                                                                                               |
+=========+=======================================================================================================================================+
| 1.2.3   | - Added missing getter methods for :java:ref:`TableLockInfo`                                                                          |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.2.2   | - Added :java:meth:`ColumnStoreDriver.clearTableLock`                                                                                 |
|         | - Added :java:meth:`ColumnStoreDriver.isTableLocked`                                                                                  |
|         | - Added :java:meth:`ColumnStoreDriver.listTableLocks`                                                                                 |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.6   | - Java documentation added                                                                                                            |
|         | - Added :java:meth:`ColumnStoreDriver.getJavaMcsapiVersion` to return version information about the java wrapper library              |
|         |   javamcsapi.jar                                                                                                                      |
|         | - Windows support added (Alpha)                                                                                                       |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.5   | - Added overloaded functions of :java:meth:`ColumnStoreBulkInsert.setColumn` and :java:meth:`ColumnStoreBulkInsert.setNull`           |
|         |   to return additional status information                                                                                             |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.4   | - Make :java:meth:`ColumnStoreSystemCatalog.getTable` and :java:meth:`ColumnStoreSystemCatalogTable.getColumn` case insensitive       |
|         | - Add :java:meth:`ColumnStoreDriver.setDebug` to enable debugging output to stderr                                                    |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.1   | - Add :java:meth:`ColumnStoreBulkInsert.isActive`                                                                                     |
|         | - Make :java:meth:`ColumnStoreBulkInsert.rollback` fail without error                                                                 |
|         | - Add :java:meth:`ColumnStoreBulkInsert.resetRow`                                                                                     |
|         | - :java:type:`ColumnStoreDateTime` now uses uint32_t for every parameter                                                              |
|         | - :java:type:`ColumnStoreSystemCatalog` now uses const for the sub-class strings                                                      |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.0β  | - First beta release                                                                                                                  |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
