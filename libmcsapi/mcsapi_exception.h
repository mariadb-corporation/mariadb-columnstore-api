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

#include <stdexcept>

namespace mcsapi
{
class MCS_API ColumnStoreError: public std::runtime_error
{
public:
    ColumnStoreError(const std::string& msg) :
        std::runtime_error(msg)
    { }
};

class MCS_API ColumnStoreBufferError: public ColumnStoreError
{
public:
    ColumnStoreBufferError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreServerError: public ColumnStoreError
{
public:
    ColumnStoreServerError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreNetworkError: public ColumnStoreError
{
public:
    ColumnStoreNetworkError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreDataError: public ColumnStoreError
{
public:
    ColumnStoreDataError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreUsageError: public ColumnStoreError
{
public:
    ColumnStoreUsageError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreConfigError: public ColumnStoreError
{
public:
    ColumnStoreConfigError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreVersionError: public ColumnStoreError
{
public:
    ColumnStoreVersionError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreInternalError: public ColumnStoreError
{
public:
    ColumnStoreInternalError(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

class MCS_API ColumnStoreNotFound: public ColumnStoreError
{
public:
    ColumnStoreNotFound(const std::string& msg) :
        ColumnStoreError(msg)
    { }
};

}
