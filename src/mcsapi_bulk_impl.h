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
class ColumnStoreBulkInsertImpl
{
public:
    ColumnStoreBulkInsertImpl(std::string& iDb, std::string& iTable, uint8_t iMode, uint16_t iPm):
        driver(nullptr),
        systemCatalog(nullptr),
        tbl(nullptr),
        commands(nullptr),
        db(iDb),
        table(iTable),
        mode(iMode),
        pm(iPm),
        uniqueId(0),
        tblLock(0),
        txnId(0),
        sessionId(65535), // Maybe change this later?
        row(nullptr),
        batchSize(10000),
        autoRollback(true)
    {
        summary = new ColumnStoreSummary();
    }
    ~ColumnStoreBulkInsertImpl();

    ColumnStoreDriverImpl* driver;
    ColumnStoreSystemCatalog* systemCatalog;
    ColumnStoreSystemCatalogTable* tbl;
    ColumnStoreTableData tableData;
    ColumnStoreCommands* commands;
    std::string db;
    std::string table;
    uint8_t mode;
    uint16_t pm;
    uint64_t uniqueId;
    uint64_t tblLock;
    uint32_t txnId;
    uint32_t sessionId;
    ColumnStoreRowData* row;
    uint32_t batchSize;
    ColumnStoreSummary* summary;
    bool autoRollback;

    void connect();
    static void onCloseWalk(uv_handle_t* handle, void *arg);
};
}
