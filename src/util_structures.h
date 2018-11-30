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

class ColumnStoreTxnID
{
public:
    ColumnStoreTxnID() :
        id(0),
        isValid(false)
    { }
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
    void setData(const boost::string_ref& val)
    {
        varbyte.assign(val.data(), val.length());
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
    ColumnStoreColumnData(uint32_t cOid, const std::string& cColumnName) :
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

/*class ColumnStoreCPInfo
{
    uint64_t lbid;
    uint64_t max;
    uint64_t min;
    uint64_t seq; // 32bit data but 64bit for alignment
};*/
}
