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

namespace mcsapi
{
ColumnStoreDateTime::ColumnStoreDateTime()
{
    mImpl = new ColumnStoreDateTimeImpl();
}

ColumnStoreDateTime::ColumnStoreDateTime(tm& time)
{
    mImpl = new ColumnStoreDateTimeImpl();
    set(time);
}

ColumnStoreDateTime::ColumnStoreDateTime(std::string& dateTime, std::string& format)
{
    mImpl = new ColumnStoreDateTimeImpl();
    set(dateTime, format);
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

bool ColumnStoreDateTime::set(std::string& dateTime, std::string& format)
{
    tm time = {};
    std::istringstream ss(dateTime);
    ss >> std::get_time(&time, format.c_str());
    if (ss.fail())
    {
        return false;
    }

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

columnstore_data_convert_status_t ColumnStoreDateTimeImpl::setFromString(std::string& dateStr)
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
                else if (day > 28)
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
}
