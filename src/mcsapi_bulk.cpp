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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "mcsapi_driver_impl.h"
#include "mcsapi_bulk_impl.h"

namespace mcsapi
{

ColumnStoreBulkInsert::ColumnStoreBulkInsert(ColumnStoreDriverImpl* driverInstance,
    std::string& db, std::string& table, uint8_t mode, uint16_t pm)
{
    mImpl = new ColumnStoreBulkInsertImpl(db, table, mode, pm);
    mImpl->driver = driverInstance;
    mImpl->connect();
}

ColumnStoreBulkInsert::~ColumnStoreBulkInsert()
{
    delete mImpl;
}

uint16_t ColumnStoreBulkInsert::getColumnCount()
{
    return 0;
}

void ColumnStoreBulkInsert::setBatchSize(uint32_t batchSize)
{
    mImpl->batchSize = batchSize;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, std::string& value, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint64_t value, columnstore_data_convert_status_t* status)
{
    // TODO: check columnNumber not > column count

    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    // TODO: apply convert_status to counters

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int64_t value, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, double value, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDateTime& value, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDecimal& value, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(column, cont, value);
    if (status)
    {
        *status = convert_status;
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setNull(uint16_t columnNumber, columnstore_data_convert_status_t* status)
{
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn* column = mImpl->tbl->columns[columnNumber];
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::getNull(column, cont);
    if (status)
    {
        *status = convert_status;
    }
    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::writeRow()
{
    // TODO: check size of row matches column count for table
    mImpl->tableData.nextRow();

    if (mImpl->tableData.row_number >= 100000)
    {
        mImpl->commands->weBulkInsert(1, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, &mImpl->tableData);
        mImpl->tableData.row_number = 0;
    }
    mImpl->row = mImpl->tableData.getRow();

    return this;
}

void ColumnStoreBulkInsert::commit()
{
    if (mImpl->tableData.row_number > 0)
    {
        mImpl->commands->weBulkInsert(1, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, &mImpl->tableData);
    }

    mImpl->commands->weBulkInsertEnd(1, mImpl->uniqueId, mImpl->txnId, mImpl->tbl->oid, 0);
    std::vector<uint64_t> lbids;
    std::vector<ColumnStoreHWM> hwms;
//    mImpl->commands->brmGetUncommittedLbids(mImpl->txnId, lbids);
    mImpl->commands->weGetWrittenLbids(1, mImpl->uniqueId, mImpl->txnId, lbids);
    mImpl->commands->weClose(1);

    mImpl->uniqueId = mImpl->commands->brmGetUniqueId();
    mImpl->commands->weKeepAlive(1);
    mImpl->commands->weBulkCommit(1, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, mImpl->tbl->oid, hwms);
    mImpl->commands->brmSetHWMAndCP(hwms, lbids, mImpl->txnId);
    mImpl->commands->brmCommitted(mImpl->txnId);
    mImpl->commands->brmTakeSnapshot();
    mImpl->commands->brmChangeState(mImpl->tblLock);
    mImpl->commands->weRemoveMeta(1, mImpl->uniqueId, mImpl->tbl->oid);
    mImpl->commands->weClose(1);
    mImpl->commands->brmReleaseTableLock(mImpl->tblLock);
}

void ColumnStoreBulkInsert::rollback()
{
}

ColumnStoreSummary* ColumnStoreBulkInsert::getSummary()
{
    return NULL;
}

void ColumnStoreBulkInsert::setTruncateIsError(bool set)
{
    (void) set;
}

/* Private parts of API below here */

ColumnStoreBulkInsertImpl::~ColumnStoreBulkInsertImpl()
{
    delete systemCatalog;
    delete commands;
}

void ColumnStoreBulkInsertImpl::connect()
{
    commands = new ColumnStoreCommands(driver);
    // TODO: support more dbRoots
    std::vector<uint32_t> dbRoots;
    dbRoots.push_back(1);

    if (!commands->procMonCheckVersion())
    {
        std::string err("Incompatible ColumnStore version found");
        throw ColumnStoreException(err);
    }
    systemCatalog = commands->brmGetSystemCatalog();
    for (auto& itTable : systemCatalog->tables)
    {
        if ((db == itTable->schema) && (table == itTable->table))
        {
            tbl = itTable;
            break;
        }
    }
    if (!tbl)
    {
        std::string err("Table not found: ");
        err.append(db);
        err.append(".");
        err.append(table);
        throw ColumnStoreException(err);
    }
    tableData.tableName = tbl->table;
    tableData.tableSchema = tbl->schema;
    for (auto& itColumn : tbl->columns)
    {
        tableData.columns.push_back(ColumnStoreColumnData(itColumn->oid, itColumn->column));
    }
    txnId = commands->brmGetTxnID(sessionId);
    uniqueId = commands->brmGetUniqueId();
    tblLock = commands->brmGetTableLock(tbl->oid, sessionId, txnId, dbRoots);
    commands->weKeepAlive(1);
    row = tableData.getRow();;
}

}
