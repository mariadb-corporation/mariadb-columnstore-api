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
class ColumnStoreBulkInsert;
class ColumnStoreAvroInsert;

static std::string DEFAULT_PATH = "/usr/local/mariadb/columnstore/etc/Columnstore.xml";

class ColumnStoreDriverImpl;
class MCS_API ColumnStoreDriver
{
public:
    ColumnStoreDriver(const std::string& path);
    // TODO: use non-root env variable then default path
    ColumnStoreDriver() : ColumnStoreDriver(DEFAULT_PATH) { };
    ~ColumnStoreDriver();

    const char* getVersion();
    ColumnStoreBulkInsert* createBulkInsert(std::string& db,
            std::string& table, uint8_t mode, uint16_t pm);

    ColumnStoreAvroInsert* createAvroInsert(std::string& db, std::string& table,
            std::string& schemaFile);

private:
    ColumnStoreDriverImpl* mImpl;
};

}