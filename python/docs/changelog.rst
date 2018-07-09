Version History
===============

This is a version history of Python API interface changes. It does not include internal fixes and changes.

+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| Version | Changes                                                                                                                               |
+=========+=======================================================================================================================================+
| 1.1.6   | - Python documentation added                                                                                                          |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.5   | - Changed the return type of :py:func:`ColumnStoreBulkInsert::writeRow` to return a List of return status and ColumnStoreBulkInsert   |
|         |   object. Before only the ColumnStoreBulkInsert object was returned.                                                                  |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.4   | - Make :py:func:`ColumnStoreSystemCatalogColumn::getColumn` and :py:func:`ColumnStoreSystemCatalogTable::getTable` case insensitive   |
|         | - Add :py:func:`ColumnStoreDriver::setDebug` to enable debugging output to stderr                                                     |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.1   | - Add :py:func:`ColumnStoreBulkInsert::isActive`                                                                                      |
|         | - Make :py:func:`ColumnStoreBulkInsert::rollback` fail without error                                                                  |
|         | - Add :py:func:`ColumnStoreBulkInsert::resetRow`                                                                                      |
|         | - :py:func:`ColumnStoreDateTime::ColumnStoreDateTime` now uses uint32_t for every parameter                                           |
|         | - :py:class:`ColumnStoreSystemCatalog` now uses const for the sub-class strings                                                       |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.0β  | - First beta release                                                                                                                  |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
