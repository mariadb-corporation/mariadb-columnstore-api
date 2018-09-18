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

#include "common.h"
#include "mcsapi_types_impl.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cctype>
#include <boost/algorithm/string.hpp>

namespace mcsapi
{
ColumnStoreDateTime::ColumnStoreDateTime()
{
    mImpl = new ColumnStoreDateTimeImpl();
}

ColumnStoreDateTime::ColumnStoreDateTime(tm& time)
{
    mImpl = new ColumnStoreDateTimeImpl();
    if (!set(time))
    {
        std::string errmsg("Invalid date/time provided in the time struct");
        throw ColumnStoreDataError(errmsg);
    }
}

ColumnStoreDateTime::ColumnStoreDateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second, uint32_t microsecond)
{
    mImpl = new ColumnStoreDateTimeImpl((uint16_t)year, (uint8_t)month, (uint8_t)day, (uint8_t)hour, (uint8_t)minute, (uint8_t)second, microsecond);
    if (!mImpl->validateDate())
    {
        std::string errmsg("A valid date/time could not be extracted from the time parameters");
        throw ColumnStoreDataError(errmsg);
    }

}

ColumnStoreDateTime::ColumnStoreDateTime(const std::string& dateTime, const std::string& format)
{
    mImpl = new ColumnStoreDateTimeImpl();
    if (!set(dateTime, format))
    {
        std::string errmsg("A valid date/time could not be extracted from the following string with the supplied format: ");
        errmsg.append(dateTime);
        throw ColumnStoreDataError(errmsg);
    }
}

ColumnStoreDateTime::~ColumnStoreDateTime()
{
    delete mImpl;
}

bool ColumnStoreDateTime::set(tm& time)
{
    mImpl->year = time.tm_year;
    if ((mImpl->year < 1000) && (mImpl->year != 0))
        mImpl->year+= 1900;
    mImpl->month = time.tm_mon + 1;
    mImpl->day = time.tm_mday;
    mImpl->hour = time.tm_hour;
    mImpl->minute = time.tm_min;
    mImpl->second = time.tm_sec;

    return mImpl->validateDate();
}

bool ColumnStoreDateTime::set(const std::string& dateTime, const std::string& format)
{
    tm time = tm();
    strptime(dateTime.c_str(), format.c_str(), &time);
    return set(time);
}

uint32_t ColumnStoreDateTimeImpl::getDateInt()
{
    uint32_t ret = 0;
    ret = year << 16;
    ret |= (month << 12);
    ret |= (day << 6);
    ret |= 0x3E;

    return ret;
}

uint64_t ColumnStoreDateTimeImpl::getDateTimeInt()
{
    uint64_t ret = 0;
    ret = (uint64_t)year << 48;
    ret |= ((uint64_t)month << 44);
    ret |= ((uint64_t)day << 38);
    ret |= ((uint64_t)hour << 32);
    ret |= ((uint64_t)minute << 26);
    ret |= ((uint64_t)second << 20);
    ret |= microsecond & 0xFFFFF;

    return ret;
}

void ColumnStoreDateTimeImpl::getDateTimeStr(std::string& sDateTime)
{
    char dateTime[20];

    snprintf(dateTime, 20, "%.4" PRIu16 "-%.2" PRIu8 "-%.2" PRIu8 " %.2" PRIu8 ":%.2" PRIu8 ":%.2" PRIu8, year, month, day, hour, minute, second);
    sDateTime = dateTime;
}

columnstore_data_convert_status_t ColumnStoreDateTimeImpl::setFromString(const std::string& dateStr)
{
    int resLen;

    resLen = sscanf(dateStr.c_str(), "%" SCNu16 "-%" SCNu8 "-%" SCNu8 " %" SCNu8 ":%" SCNu8 ":%" SCNu8 ".%" SCNu32, &year, &month, &day, &hour, &minute, &second, &microsecond);
    if ((resLen != 3) && (resLen != 6) && (resLen != 7))
    {
        return CONVERT_STATUS_INVALID;
    }

    if (!validateDate())
        return CONVERT_STATUS_INVALID;

    return CONVERT_STATUS_NONE;
}

bool ColumnStoreDateTimeImpl::validateDate()
{
    if (year > 9999)
    {
        return false;
    }

    // Y-m-d 0000-00-00 is valid
    if ((year != 0) || (month != 0) || (day != 0))
    {
        if ((month < 1) || (month > 12))
        {
            return false;
        }

        if (day < 1)
        {
            return false;
        }

        bool leap;
        if (year % 4 == 0)
        {
            if (year % 100 == 0)
            {
                if (year % 400 == 0)
                {
                    leap = true;
                }
                else
                {
                    leap = false;
                }
            }
            else
            {
                leap = true;
            }
        }
        else
        {
            leap = false;
        }
        switch (month)
        {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                if (day > 31)
                    return false;
                break;
            case 2:
                if ((leap) && (day > 29))
                    return false;
                else if (!leap && day > 28)
                    return false;
                break;
            default:
                if (day > 30)
                    return false;
                break;
        }
    }

    if (hour > 23)
        return false;

    if (minute > 59)
        return false;

    // We don't support leap seconds
    if (second > 59)
        return false;

    return true;
}

ColumnStoreDecimal::ColumnStoreDecimal()
{
    mImpl = new ColumnStoreDecimalImpl();
}

ColumnStoreDecimal::ColumnStoreDecimal(int64_t value)
{
    mImpl = new ColumnStoreDecimalImpl();
    if (!set(value))
    {
        std::string errmsg("Error converting value to Decimal: ");
        errmsg += std::to_string(value);
        throw ColumnStoreDataError(errmsg);
    }
}

ColumnStoreDecimal::ColumnStoreDecimal(const std::string& value)
{
    mImpl = new ColumnStoreDecimalImpl();
    if (!set(value))
    {
        std::string errmsg("Error converting value to Decimal: ");
        errmsg += value;
        throw ColumnStoreDataError(errmsg);
    }
}

ColumnStoreDecimal::ColumnStoreDecimal(double value)
{
    mImpl = new ColumnStoreDecimalImpl();
    if (!set(value))
    {
        std::string errmsg("Error converting value to Decimal: ");
        errmsg += std::to_string(value);
        throw ColumnStoreDataError(errmsg);
    }
}

ColumnStoreDecimal::ColumnStoreDecimal(int64_t number, uint8_t scale)
{
    mImpl = new ColumnStoreDecimalImpl();
    if (!set(number, scale))
    {
        std::string errmsg("Error converting value to Decimal: ");
        errmsg += std::to_string(number) + ", scale: " + std::to_string(scale);
        throw ColumnStoreDataError(errmsg);
    }

}

ColumnStoreDecimal::~ColumnStoreDecimal()
{
    delete mImpl;
}


bool ColumnStoreDecimal::set(int64_t value)
{
    mImpl->decNum = value;
    return true;
}

bool ColumnStoreDecimal::set(const std::string& value)
{
    mImpl->decNum.assign(value);
    return true;
}

bool ColumnStoreDecimal::set(double value)
{
    mImpl->decNum = value;
    return true;
}

bool ColumnStoreDecimal::set(int64_t number, uint8_t scale)
{
    mImpl->decNum = number;
    mImpl->decNum *= boost::multiprecision::pow(Decimal18(10), 0 - scale);
    return true;
}

uint64_t ColumnStoreDecimalImpl::getDecimalInt(uint32_t scale)
{
    uint64_t result;
    // MCOL-1444 Boost 1.65 does something weird with zero, so we shortcut
    if (decNum == 0)
    {
        return 0;
    }
    int64_t decimalScale = boost::multiprecision::ilogb(decNum);
    boost::multiprecision::cpp_dec_float_50 converted = decNum * pow(10, 18 - decimalScale);
    decimalScale = 18 - decimalScale;

    if (scale > decimalScale)
    {
        converted *= boost::multiprecision::pow(Decimal18(10), scale - decimalScale);
    }
    else if (scale < decimalScale)
    {
        converted /= boost::multiprecision::pow(Decimal18(10), decimalScale - scale);
    }
    result = static_cast<int64_t>(converted);

    return result;
}

int64_t ColumnStoreDecimalImpl::getInt()
{
    int64_t result = llround(getDouble());
    return result;
}

double ColumnStoreDecimalImpl::getDouble()
{
    double result = decNum.convert_to<double>();
    return result;
}

void ColumnStoreDecimalImpl::getDecimalStr(std::string& sDecimal)
{
    sDecimal = decNum.str(18);
}

ColumnStoreSummary::ColumnStoreSummary()
{
    mImpl = new ColumnStoreSummaryImpl();
}

ColumnStoreSummary::ColumnStoreSummary(const ColumnStoreSummary& summary)
{
    mImpl = new ColumnStoreSummaryImpl();
    *mImpl = *summary.mImpl;
}

ColumnStoreSummary& ColumnStoreSummary::operator=(const ColumnStoreSummary& summary)
{
    *mImpl = *summary.mImpl;
    return *this;
}

ColumnStoreSummary::~ColumnStoreSummary()
{
    delete mImpl;
}

double ColumnStoreSummary::getExecutionTime()
{
    return mImpl->getExecTime();
}

uint64_t ColumnStoreSummary::getRowsInsertedCount()
{
    return mImpl->insertedCount;
}

uint64_t ColumnStoreSummary::getTruncationCount()
{
    return mImpl->truncatedCount;
}

uint64_t ColumnStoreSummary::getInvalidCount()
{
    return mImpl->invalidCount;
}

uint64_t ColumnStoreSummary::getSaturatedCount()
{
    return mImpl->saturatedCount;
}

void ColumnStoreSummaryImpl::setStatus(columnstore_data_convert_status_t status)
{
    switch (status)
    {
        case CONVERT_STATUS_NONE:
            break;
        case CONVERT_STATUS_INVALID:
            invalidCount++;
            break;
        case CONVERT_STATUS_SATURATED:
            saturatedCount++;
            break;
        case CONVERT_STATUS_TRUNCATED:
            truncatedCount++;
            break;
        default:
            break;
    }
}

ColumnStoreSystemCatalog::ColumnStoreSystemCatalog()
{
    mImpl = new ColumnStoreSystemCatalogImpl();
}

ColumnStoreSystemCatalog::ColumnStoreSystemCatalog(const ColumnStoreSystemCatalog& obj)
{
    mImpl = new ColumnStoreSystemCatalogImpl();
    if (obj.mImpl)
    {
        mImpl->tables = obj.mImpl->tables;
    }
}

ColumnStoreSystemCatalog::~ColumnStoreSystemCatalog()
{
    delete mImpl;
}

ColumnStoreSystemCatalogColumn::ColumnStoreSystemCatalogColumn()
{
    mImpl = new ColumnStoreSystemCatalogColumnImpl();
}

ColumnStoreSystemCatalogColumn::ColumnStoreSystemCatalogColumn(const ColumnStoreSystemCatalogColumn& obj)
{
    mImpl = new ColumnStoreSystemCatalogColumnImpl();
    if (obj.mImpl)
    {
        mImpl->autoincrement = obj.mImpl->autoincrement;
        mImpl->column = obj.mImpl->column;
        mImpl->compression = obj.mImpl->compression;
        mImpl->default_val = obj.mImpl->default_val;
        mImpl->dict_oid = obj.mImpl->dict_oid;
        mImpl->null = obj.mImpl->null;
        mImpl->oid = obj.mImpl->oid;
        mImpl->position = obj.mImpl->position;
        mImpl->precision = obj.mImpl->precision;
        mImpl->scale = obj.mImpl->scale;
        mImpl->type = obj.mImpl->type;
        mImpl->width = obj.mImpl->width;
    }
}

ColumnStoreSystemCatalogColumn::~ColumnStoreSystemCatalogColumn()
{
    delete mImpl;
}

ColumnStoreSystemCatalogTable::ColumnStoreSystemCatalogTable()
{
    mImpl = new ColumnStoreSystemCatalogTableImpl();
}

ColumnStoreSystemCatalogTable::ColumnStoreSystemCatalogTable(const ColumnStoreSystemCatalogTable& obj)
{
    mImpl = new ColumnStoreSystemCatalogTableImpl();
    if (obj.mImpl)
    {
        mImpl->columns = obj.mImpl->columns;
        mImpl->oid = obj.mImpl->oid;
        mImpl->schema = obj.mImpl->schema;
        mImpl->table = obj.mImpl->table;
    }
}


ColumnStoreSystemCatalogTable::~ColumnStoreSystemCatalogTable()
{
    delete mImpl;
}

uint32_t ColumnStoreSystemCatalogColumn::getOID()
{
    return mImpl->oid;
}

const std::string& ColumnStoreSystemCatalogColumn::getColumnName()
{
    return mImpl->column;
}

uint32_t ColumnStoreSystemCatalogColumn::getDictionaryOID()
{
    return mImpl->dict_oid;
}

columnstore_data_types_t ColumnStoreSystemCatalogColumn::getType()
{
    return mImpl->type;
}

uint32_t ColumnStoreSystemCatalogColumn::getWidth()
{
    return mImpl->width;
}

uint32_t ColumnStoreSystemCatalogColumn::getPosition()
{
    return mImpl->position;
}

const std::string& ColumnStoreSystemCatalogColumn::getDefaultValue()
{
    return mImpl->default_val;
}

bool ColumnStoreSystemCatalogColumn::isAutoincrement()
{
    return mImpl->autoincrement;
}

uint32_t ColumnStoreSystemCatalogColumn::getPrecision()
{
    return mImpl->precision;
}

uint32_t ColumnStoreSystemCatalogColumn::getScale()
{
    return mImpl->scale;
}

bool ColumnStoreSystemCatalogColumn::isNullable()
{
    return mImpl->null;
}

uint8_t ColumnStoreSystemCatalogColumn::compressionType()
{
    return mImpl->compression;
}

const std::string& ColumnStoreSystemCatalogTable::getSchemaName()
{
    return mImpl->schema;
}

const std::string& ColumnStoreSystemCatalogTable::getTableName()
{
    return mImpl->table;
}

uint32_t ColumnStoreSystemCatalogTable::getOID()
{
    return mImpl->oid;
}

uint16_t ColumnStoreSystemCatalogTable::getColumnCount()
{
    return mImpl->columns.size();
}

ColumnStoreSystemCatalogColumn& ColumnStoreSystemCatalogTable::getColumn(const std::string& columnName)
{
    ColumnStoreSystemCatalogColumn* col = nullptr;
    for (auto& itColumn : mImpl->columns)
    {
        if (boost::iequals(columnName,itColumn->getColumnName()))
        {
            col = itColumn;
            break;
        }
    }
    if (col)
    {
        return *col;
    }
    std::string err = getSchemaName() + "." + getTableName() + "." + columnName + " not found";
    throw ColumnStoreNotFound(err);
}

ColumnStoreSystemCatalogColumn& ColumnStoreSystemCatalogTable::getColumn(uint16_t columnNumber)
{
    if (columnNumber >= mImpl->columns.size())
    {
        std::string errmsg = "Column number " + std::to_string(columnNumber) + " not found";
        throw ColumnStoreNotFound(errmsg);
    }
    return *mImpl->columns[columnNumber];
}


ColumnStoreSystemCatalogTable& ColumnStoreSystemCatalog::getTable(const std::string& schemaName, const std::string& tableName)
{
    ColumnStoreSystemCatalogTable* tbl = nullptr;
    for (auto& itTable : mImpl->tables)
    {
        if (boost::iequals(schemaName, itTable->getSchemaName()) && boost::iequals(tableName, itTable->getTableName()))
        {
            tbl = itTable;
            break;
        }
    }
    if (tbl)
    {
        return *tbl;
    }
    std::string err = schemaName + "." + tableName + " not found";
    throw ColumnStoreNotFound(err);
}

void ColumnStoreSystemCatalogTableImpl::clear()
{
    for (std::vector<ColumnStoreSystemCatalogColumn*>::iterator it = columns.begin() ; it != columns.end(); ++it)
    {
        delete *it;
    }
}

void ColumnStoreSystemCatalogImpl::clear()
{
    for (std::vector<ColumnStoreSystemCatalogTable*>::iterator it = tables.begin() ; it != tables.end(); ++it)
    {
        (*it)->mImpl->clear();
        delete *it;
    }
}

}
