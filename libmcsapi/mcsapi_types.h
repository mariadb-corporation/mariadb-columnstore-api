/* Copyright (c) 2017, MariaDB Corporation. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301  USA
 */

#pragma once

namespace mcsapi
{


class ColumnStoreDateTimeImpl;
class ColumnStoreDecimalImpl;
class ColumnStoreSummaryImpl;
class ColumnStoreSystemCatalogImpl;
class ColumnStoreSystemCatalogTableImpl;
class ColumnStoreSystemCatalogColumnImpl;
class ColumnStoreDataConvert;
class MCS_API ColumnStoreDateTime
{
    friend class ColumnStoreDataConvert;
public:
    ColumnStoreDateTime();
    ColumnStoreDateTime(tm& time);
    ColumnStoreDateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour = 0, uint32_t minute = 0, uint32_t second = 0, uint32_t microsecond = 0);
    ColumnStoreDateTime(const std::string& dateTime, const std::string& format);

    ~ColumnStoreDateTime();
    bool set(tm& time);
    bool set(const std::string& dateTime, const std::string& format);
private:
    ColumnStoreDateTimeImpl* mImpl;
};

class MCS_API ColumnStoreDecimal
{
    friend class ColumnStoreDataConvert;
public:
    ColumnStoreDecimal();
    ColumnStoreDecimal(int64_t value);
    ColumnStoreDecimal(const std::string& value);
    ColumnStoreDecimal(double value);
    ColumnStoreDecimal(int64_t number, uint8_t scale);

    ~ColumnStoreDecimal();
    bool set(int64_t value);
    bool set(const std::string& value);
    bool set(double value);
    bool set(int64_t number, uint8_t scale);
private:
    ColumnStoreDecimalImpl* mImpl;
};

class MCS_API ColumnStoreSummary
{
    friend class ColumnStoreBulkInsert;
public:
    ColumnStoreSummary();
    ColumnStoreSummary(const ColumnStoreSummary& summary);
    ~ColumnStoreSummary();
    ColumnStoreSummary& operator=(const ColumnStoreSummary& summary);
    double getExecutionTime();
    uint64_t getRowsInsertedCount();
    uint64_t getTruncationCount();
    uint64_t getSaturatedCount();
    uint64_t getInvalidCount();
private:
    ColumnStoreSummaryImpl* mImpl;
};

enum MCS_API columnstore_data_types_t
{
    DATA_TYPE_BIT,
    DATA_TYPE_TINYINT,
    DATA_TYPE_CHAR,
    DATA_TYPE_SMALLINT,
    DATA_TYPE_DECIMAL,
    DATA_TYPE_MEDINT,
    DATA_TYPE_INT,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DATE,
    DATA_TYPE_BIGINT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_DATETIME,
    DATA_TYPE_VARCHAR,
    DATA_TYPE_VARBINARY,
    DATA_TYPE_CLOB,
    DATA_TYPE_BLOB,
    DATA_TYPE_UTINYINT,
    DATA_TYPE_USMALLINT,
    DATA_TYPE_UDECIMAL,
    DATA_TYPE_UMEDINT,
    DATA_TYPE_UINT,
    DATA_TYPE_UFLOAT,
    DATA_TYPE_UBIGINT,
    DATA_TYPE_UDOUBLE,
    DATA_TYPE_TEXT
};

class MCS_API ColumnStoreSystemCatalogColumn
{
    friend class ColumnStoreCommands;
public:
    ColumnStoreSystemCatalogColumn();
    ColumnStoreSystemCatalogColumn(const ColumnStoreSystemCatalogColumn& obj);
    ~ColumnStoreSystemCatalogColumn();
    uint32_t getOID();
    const std::string& getColumnName();
    uint32_t getDictionaryOID();
    columnstore_data_types_t getType();
    uint32_t getWidth();
    uint32_t getPosition();
    const std::string& getDefaultValue();
    bool isAutoincrement();
    uint32_t getPrecision();
    uint32_t getScale();
    bool isNullable();
    uint8_t compressionType();

private:
    ColumnStoreSystemCatalogColumnImpl *mImpl;
};

class MCS_API ColumnStoreSystemCatalogTable
{
    friend class ColumnStoreCommands;
    friend class ColumnStoreSystemCatalogImpl;
public:
    ColumnStoreSystemCatalogTable();
    ColumnStoreSystemCatalogTable(const ColumnStoreSystemCatalogTable& obj);
    ~ColumnStoreSystemCatalogTable();
    const std::string& getSchemaName();
    const std::string& getTableName();
    uint32_t getOID();
    uint16_t getColumnCount();
    ColumnStoreSystemCatalogColumn& getColumn(const std::string& columnName);
    ColumnStoreSystemCatalogColumn& getColumn(uint16_t columnNumber);
private:
    ColumnStoreSystemCatalogTableImpl* mImpl;
};

class MCS_API ColumnStoreSystemCatalog
{
    friend class ColumnStoreCommands;
    friend class ColumnStoreDriverImpl;
public:
    ColumnStoreSystemCatalog();
    ColumnStoreSystemCatalog(const ColumnStoreSystemCatalog& obj);
    ~ColumnStoreSystemCatalog();
    ColumnStoreSystemCatalogTable& getTable(const std::string& schemaName, const std::string& tableName);
private:
    ColumnStoreSystemCatalogImpl* mImpl;
};

}
