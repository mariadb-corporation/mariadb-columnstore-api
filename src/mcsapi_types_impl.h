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
class ColumnStoreDateTimeImpl
{
public:
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint32_t microsecond; // Unused, for a later date
    ColumnStoreDateTimeImpl() :
        year(0),
        month(0),
        day(0),
        hour(0),
        minute(0),
        second(0),
        microsecond(0)
    { }
    uint32_t getDateInt();
    uint64_t getDateTimeInt();
    void getDateTimeStr(std::string& sDateTime);
    columnstore_data_convert_status_t setFromString(std::string& dateStr);
    bool validateDate();
};

class ColumnStoreDecimalImpl
{
public:
    int64_t decimalNumber;
    uint8_t decimalScale;
    ColumnStoreDecimalImpl() :
        decimalNumber(0),
        decimalScale(0)
    { }

    uint64_t getDecimalInt(uint32_t scale);
    int64_t getInt();
    double getDouble();
    void getDecimalStr(std::string& sDecimal);
};

class ColumnStoreSummaryImpl
{
public:
    uint64_t invalidCount;
    uint64_t truncatedCount;
    uint64_t insertedCount;
    uint64_t saturatedCount;
    std::clock_t start;
    std::clock_t end;
    ColumnStoreSummaryImpl() :
        invalidCount(0),
        truncatedCount(0),
        insertedCount(0),
        saturatedCount(0)
    { }

    void startTimer()
    {
        start = std::clock();
    }

    void stopTimer()
    {
        end = std::clock();
    }

    double getExecTime()
    {
        return (end - start) / (double)CLOCKS_PER_SEC;
    }

    void setStatus(columnstore_data_convert_status_t status);
};
}
