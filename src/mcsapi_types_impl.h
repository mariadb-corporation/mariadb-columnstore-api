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

#include <chrono>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace mcsapi
{

typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<18> > Decimal18;

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

    ColumnStoreDateTimeImpl(uint16_t p_year, uint8_t p_month, uint8_t p_day, uint8_t p_hour, uint8_t p_minute, uint8_t p_second, uint32_t p_microsecond)
    {
        this->year = p_year;
        this->month = p_month;
        this->day = p_day;
        this->hour = p_hour;
        this->minute = p_minute;
        this->second = p_second;
        this->microsecond = p_microsecond;
    }
    
    uint32_t getDateInt();
    uint64_t getDateTimeInt();
    void getDateTimeStr(std::string& sDateTime);
    columnstore_data_convert_status_t setFromString(const std::string& dateStr);
    bool validateDate();
};

class ColumnStoreDecimalImpl
{
public:
    boost::multiprecision::cpp_dec_float_50 decNum;
    ColumnStoreDecimalImpl() :
        decNum(0)
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

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    ColumnStoreSummaryImpl() :
        invalidCount(0),
        truncatedCount(0),
        insertedCount(0),
        saturatedCount(0)
    { }

    void startTimer()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    void stopTimer()
    {
        end = std::chrono::high_resolution_clock::now();
    }

    double getExecTime()
    {
        return std::chrono::duration<double>(end-start).count();
    }

    void setStatus(columnstore_data_convert_status_t status);
};

class ColumnStoreSystemCatalogColumnImpl
{
public:
    ColumnStoreSystemCatalogColumnImpl() :
        oid(0),
        column(""),
        dict_oid(0),
        type(DATA_TYPE_BIT),
        width(0),
        position(0),
        default_val(""),
        autoincrement(0),
        precision(0),
        scale(0),
        null(0),
        compression(0)
    {}
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

class ColumnStoreSystemCatalogTableImpl
{
public:
    ColumnStoreSystemCatalogTableImpl() :
        oid(0)
    {}
    void clear();
    uint32_t oid;
    std::string schema;
    std::string table;
    std::vector<ColumnStoreSystemCatalogColumn*> columns;
};

class ColumnStoreSystemCatalogImpl
{
public:
    void clear();
    std::vector<ColumnStoreSystemCatalogTable*> tables;
};

}
