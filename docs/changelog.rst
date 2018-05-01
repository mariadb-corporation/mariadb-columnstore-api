Version History
===============

This is a version history of C++ API interface changes. It does not include internal fixes and changes.

+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| Version | Changes                                                                                                                               |
+=========+=======================================================================================================================================+
| 1.2.0   | - Add support for TIME datatype with :cpp:class:`ColumnStoreTime`                                                                     |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.4   | - Make :cpp:func:`ColumnStoreSystemCatalogColumn::getColumn` and :cpp:func:`ColumnStoreSystemCatalogTable::getTable` case insensitive |
|         | - Add :cpp:func:`ColumnStoreDriver::setDebug` to enable debugging output to stderr                                                    |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.1   | - Add :cpp:func:`ColumnStoreBulkInsert::isActive`                                                                                     |
|         | - Make :cpp:func:`ColumnStoreBulkInsert::rollback` fail without error                                                                 |
|         | - Add :cpp:func:`ColumnStoreBulkInsert::resetRow`                                                                                     |
|         | - :cpp:func:`ColumnStoreDateTime::ColumnStoreDateTime` now uses uint32_t for every parameter                                          |
|         | - :cpp:class:`ColumnStoreSystemCatalog` now uses const for the sub-class strings                                                      |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
| 1.1.0β  | - First beta release                                                                                                                  |
+---------+---------------------------------------------------------------------------------------------------------------------------------------+
