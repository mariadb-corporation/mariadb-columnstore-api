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

enum columnstore_data_convert_status_t
{
    CONVERT_STATUS_NONE,
    CONVERT_STATUS_SATURATED,
    CONVERT_STATUS_INVALID,
    CONVERT_STATUS_TRUNCATED
};

class ColumnStoreBulkInsertImpl;
class ColumnStoreDriverImpl;
class MCS_API ColumnStoreBulkInsert
{
    friend class ColumnStoreDriver;
public:
    ~ColumnStoreBulkInsert();

    uint16_t getColumnCount();
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, std::string& value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, uint64_t value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, int64_t value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, uint32_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (uint64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, int32_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (int64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, uint16_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (uint64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, int16_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (int64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, uint8_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (uint64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, int8_t value, columnstore_data_convert_status_t* status = nullptr)
    {
        return setColumn(columnNumber, (int64_t)value, status);
    }

    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, double value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, ColumnStoreDateTime& value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setColumn(uint16_t columnNumber, ColumnStoreDecimal& value, columnstore_data_convert_status_t* status = nullptr);
    ColumnStoreBulkInsert* setNull(uint16_t columnNumber);
    ColumnStoreBulkInsert* writeRow();
    void commit();
    void rollback();
    ColumnStoreSummary* getSummary();
    void setTruncateIsError(bool set);
    void setBatchSize(uint32_t batchSize);

private:
    // TODO: parameters
    ColumnStoreBulkInsert(ColumnStoreDriverImpl* driverInstance, std::string& db,
    	std::string& table, uint8_t mode, uint16_t pm);
    ColumnStoreBulkInsertImpl* mImpl;
};

}
