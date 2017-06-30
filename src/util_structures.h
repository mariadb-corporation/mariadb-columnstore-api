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

enum columnstore_data_types_t
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

enum columnstore_lock_types_t
{
    LOCK_TYPE_LOADING,
    LOCK_TYPE_CLEANUP
};

class ColumnStoreTxnID
{
public:
    uint32_t id;
    bool isValid;
};

class ColumnStoreTableLock
{
public:
    uint32_t ownerPID;
    std::string ownerName;
    uint32_t sessionID;
    uint32_t ownerTxnID;
};

class ColumnStoreDataContainer
{
public:
    void setData(uint8_t val)
    {
        byte = val;
        datatype = BYTE;
        isNull = false;
    }
    void setData(uint16_t val)
    {
        duobyte = val;
        datatype = DUOBYTE;
        isNull = false;
    }
    void setData(uint32_t val)
    {
        quadbyte = val;
        datatype = QUADBYTE;
        isNull = false;
    }
    void setData(uint64_t val)
    {
        octbyte = val;
        datatype = OCTBYTE;
        isNull = false;
    }
    void setData(std::string& val)
    {
        varbyte = val;
        datatype = VARBYTE;
        isNull = false;
    }
    void setData(int8_t val)
    {
        sbyte = val;
        datatype = BYTE;
        isNull = false;
    }
    void setData(int16_t val)
    {
        sduobyte = val;
        datatype = DUOBYTE;
        isNull = false;
    }
    void setData(int32_t val)
    {
        squadbyte = val;
        datatype = QUADBYTE;
        isNull = false;
    }
    void setData(int64_t val)
    {
        soctbyte = val;
        datatype = OCTBYTE;
        isNull = false;
    }

    union
    {
        uint8_t byte;
        int8_t sbyte;
        uint16_t duobyte;
        int16_t sduobyte;
        uint32_t quadbyte;
        int32_t squadbyte;
        uint64_t octbyte;
        int64_t soctbyte;
    };
    std::string varbyte;
    enum types
    {
        BYTE,
        DUOBYTE,
        QUADBYTE,
        OCTBYTE,
        VARBYTE
    };
    types datatype;
    bool isNull;
};

typedef std::map<uint32_t, ColumnStoreDataContainer> ColumnStoreRowData;

class ColumnStoreColumnData
{
public:
    ColumnStoreColumnData(uint32_t cOid, std::string& cColumnName) :
        oid(cOid),
        columnName(cColumnName)
    { }
    uint32_t oid;
    std::string columnName;
};

class ColumnStoreTableData
{
public:
    ColumnStoreTableData() :
        row_number(0)
    {
        rows.resize(100000);
    }

    std::string tableName;
    std::string tableSchema;
    std::vector<ColumnStoreColumnData> columns;
    std::vector<ColumnStoreRowData> rows;
    size_t row_number;

    ColumnStoreRowData* getRow()
    {
        return &rows[row_number];
    }

    void nextRow()
    {
        row_number++;
    }

    ~ColumnStoreTableData()
    {
    }
};

class ColumnStoreSystemCatalogColumn
{
public:
    uint32_t oid;
    std::string column;
    uint32_t dict_oid;
    columnstore_data_types_t type;
    uint32_t width;
    uint32_t position;
    std::string default_val;
    uint8_t autoincrement;
    uint32_t precision;
    uint32_t scale;
    uint8_t null;
    uint8_t compression;
};

class ColumnStoreSystemCatalogTable
{
public:
    ColumnStoreSystemCatalogTable() :
        oid(0)
    {}
    ~ColumnStoreSystemCatalogTable();
    uint32_t oid;
    std::string schema;
    std::string table;
    std::vector<ColumnStoreSystemCatalogColumn*> columns;
};

class ColumnStoreSystemCatalog
{
public:
    ~ColumnStoreSystemCatalog();
    std::vector<ColumnStoreSystemCatalogTable*> tables;
};

class ColumnStoreHWM
{
public:
    ColumnStoreHWM(uint32_t iOid, uint32_t iPartNum, uint32_t iSegNum, uint32_t iHwm) :
        oid(iOid),
        partNum(iPartNum),
        segNum(iSegNum),
        hwm(iHwm)
    { }
    uint32_t oid;
    uint32_t partNum;
    uint32_t segNum;
    uint32_t hwm;
};

class ColumnStoreCPInfo
{
    uint64_t lbid;
    uint64_t max;
    uint64_t min;
    uint64_t seq; // 32bit data but 64bit for alignment
};
}
