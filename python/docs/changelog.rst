Version History
===============

This is a version history of Python API interface changes. It does not include internal fixes and changes.

+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| Version | Changes                                                                                                                               |
+=========+=======================================================================================================================================+
| 1.2.2   | - Added :py:meth:`ColumnStoreDriver.listTableLocks`                                                                                   |
|         | - Added :py:meth:`ColumnStoreDriver.isTableLocked`                                                                                    |
|         | - Added :py:meth:`ColumnStoreDriver.clearTableLock`                                                                                   |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.6   | - Python documentation added                                                                                                          |
|         | - Windows support added (Alpha)                                                                                                       |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.5   | - Changed the return type of :py:meth:`ColumnStoreBulkInsert.setColumn` and :py:meth:`ColumnStoreBulkInsert.setNull`                  |
|         |   to return a List of ColumnStoreBulkInsert object and return status. Before only the ColumnStoreBulkInsert object was returned.      |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.4   | - Make :py:meth:`ColumnStoreSystemCatalog.getTable` and :py:meth:`ColumnStoreSystemCatalogTable.getColumn` case insensitive           |
|         | - Add :py:meth:`ColumnStoreDriver.setDebug` to enable debugging output to stderr                                                      |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.1   | - Add :py:meth:`ColumnStoreBulkInsert.isActive`                                                                                       |
|         | - Make :py:meth:`ColumnStoreBulkInsert.rollback` fail without error                                                                   |
|         | - Add :py:meth:`ColumnStoreBulkInsert.resetRow`                                                                                       |
|         | - :py:meth:`pymcsapi.ColumnStoreDateTime` now uses uint32_t for every parameter                                                       |
|         | - :py:class:`ColumnStoreSystemCatalog` now uses const for the sub-class strings                                                       |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.0β  | - First beta release                                                                                                                  |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+

