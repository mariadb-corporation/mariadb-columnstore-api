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

class ColumnStoreDataConvert
{
public:
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, uint64_t fromValue);
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, int64_t fromValue);
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, double fromValue);
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, const std::string& fromValue);
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, ColumnStoreDateTime& fromValue);
    static columnstore_data_convert_status_t convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, ColumnStoreDecimal& fromValue);
    static columnstore_data_convert_status_t getNull(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont);
};

}
