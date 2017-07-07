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
class ColumnStoreDataConvert;
class MCS_API ColumnStoreDateTime
{
    friend class ColumnStoreDataConvert;
public:
    ColumnStoreDateTime();
    ColumnStoreDateTime(tm& time);
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
}
