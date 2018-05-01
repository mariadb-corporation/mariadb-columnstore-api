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
#include "mcsapi_types_impl.h"

namespace mcsapi
{

ColumnStoreBulkInsert::ColumnStoreBulkInsert(ColumnStoreDriverImpl* driverInstance,
    const std::string& db, const std::string& table, uint8_t mode, uint16_t pm)
{
    mImpl = new ColumnStoreBulkInsertImpl(db, table, mode, pm);
    mImpl->driver = driverInstance;
    mImpl->connect();
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->startTimer();
}

ColumnStoreBulkInsert::~ColumnStoreBulkInsert()
{
    if (mImpl->autoRollback)
        rollback();
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

void ColumnStoreBulkInsertImpl::runChecks(uint16_t columnNumber)
{
    if (transactionClosed)
    {
        std::string errmsg = "Bulk insert has been committed or rolled back and cannot be reused";
        throw ColumnStoreUsageError(errmsg);
    }
    if (columnNumber > tbl->getColumnCount())
    {
        std::string errmsg = "Column number " + std::to_string(columnNumber) + " not valid";
        throw ColumnStoreUsageError(errmsg);
    }

}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, const std::string& value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, uint64_t value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, int64_t value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, double value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDateTime& value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreTime& value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setColumn(uint16_t columnNumber, ColumnStoreDecimal& value, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::convert(&column, cont, value);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    if (mImpl->truncateIsError && convert_status == CONVERT_STATUS_TRUNCATED)
    {
        std::string errmsg = "Column " + std::to_string(columnNumber) + " truncated";
        throw ColumnStoreDataError(errmsg);
    }

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::setNull(uint16_t columnNumber, columnstore_data_convert_status_t* status)
{
    mImpl->runChecks(columnNumber);
    columnstore_data_convert_status_t convert_status;
    ColumnStoreSystemCatalogColumn column = mImpl->tbl->getColumn(columnNumber);
    ColumnStoreDataContainer* cont = &(*mImpl->row)[columnNumber];
    convert_status = ColumnStoreDataConvert::getNull(&column, cont);
    if (status)
    {
        *status = convert_status;
    }
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->setStatus(convert_status);

    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::resetRow()
{
    if (mImpl->transactionClosed)
    {
        std::string errmsg = "Bulk insert has been committed or rolled back and cannot be reused";
        throw ColumnStoreUsageError(errmsg);
    }
    mImpl->row->clear();
    return this;
}

ColumnStoreBulkInsert* ColumnStoreBulkInsert::writeRow()
{
    if (mImpl->transactionClosed)
    {
        std::string errmsg = "Bulk insert has been committed or rolled back and cannot be reused";
        throw ColumnStoreUsageError(errmsg);
    }

    if (mImpl->row->size() != mImpl->tbl->getColumnCount())
    {
        std::string errmsg = "Not all the columns for this row have been filled";
        throw ColumnStoreUsageError(errmsg);
    }
    mImpl->tableData.nextRow();

    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;
    summaryImpl->insertedCount++;

    if (mImpl->tableData.row_number >= 100000)
    {
        uint16_t pm = mImpl->pmList[mImpl->currentPm];
        mImpl->commands->weBulkInsert(pm, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, &mImpl->tableData);
        mImpl->tableData.row_number = 0;
        if (mImpl->currentPm+1 >= mImpl->pmList.size())
        {
            mImpl->currentPm = 0;
        }
        else
        {
            mImpl->currentPm++;
        }
    }
    mImpl->row = mImpl->tableData.getRow();

    return this;
}

void ColumnStoreBulkInsert::commit()
{
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;

    if (mImpl->transactionClosed)
    {
        std::string errmsg = "Bulk insert has been already been committed or rolled back";
        throw ColumnStoreUsageError(errmsg);
    }

    if (mImpl->tableData.row_number > 0)
    {
        uint16_t pm = mImpl->pmList[mImpl->currentPm];
        mImpl->commands->weBulkInsert(pm, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, &mImpl->tableData);
    }

    mImpl->uniqueId = mImpl->commands->brmGetUniqueId();
    for (auto& pmit: mImpl->pmList)
    {
        mImpl->commands->weBulkInsertEnd(pmit, mImpl->uniqueId, mImpl->txnId, mImpl->tbl->getOID(), 0);
        std::vector<uint64_t> lbids;
        std::vector<ColumnStoreHWM> hwms;
        mImpl->commands->weGetWrittenLbids(pmit, mImpl->uniqueId, mImpl->txnId, lbids);
        mImpl->commands->weBulkCommit(pmit, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId, mImpl->tbl->getOID(), hwms);
        mImpl->commands->brmSetHWMAndCP(hwms, lbids, mImpl->txnId);
    }
    mImpl->commands->brmCommitted(mImpl->txnId);
    mImpl->commands->brmTakeSnapshot();
    mImpl->commands->brmChangeState(mImpl->tblLock);
    for (auto& pmit: mImpl->pmList)
    {
        mImpl->commands->weRemoveMeta(pmit, mImpl->uniqueId, mImpl->tbl->getOID());
        mImpl->commands->weClose(pmit);
    }
    mImpl->commands->brmReleaseTableLock(mImpl->tblLock);
    mImpl->autoRollback = false;
    mImpl->transactionClosed = true;
    summaryImpl->stopTimer();
}

bool ColumnStoreBulkInsert::isActive()
{
    return !mImpl->transactionClosed;
}

void ColumnStoreBulkInsert::rollback()
{
    ColumnStoreSummaryImpl* summaryImpl = mImpl->summary->mImpl;

    if (mImpl->transactionClosed)
    {
        return;
    }

    for (auto& pmit: mImpl->pmList)
    {
        std::vector<uint64_t> lbids;
        mImpl->commands->weGetWrittenLbids(pmit, mImpl->uniqueId, mImpl->txnId, lbids);
        mImpl->commands->weRollbackBlocks(pmit, mImpl->uniqueId, mImpl->sessionId, mImpl->txnId);
        mImpl->commands->brmRollback(lbids, mImpl->txnId);
        mImpl->commands->weBulkRollback(pmit, mImpl->uniqueId, mImpl->sessionId, mImpl->tblLock, mImpl->tbl->getOID());
    }
    mImpl->commands->brmChangeState(mImpl->tblLock);
    for (auto& pmit: mImpl->pmList)
    {
        mImpl->commands->weRemoveMeta(pmit, mImpl->uniqueId, mImpl->tbl->getOID());
        mImpl->commands->weClose(pmit);
    }
    mImpl->commands->brmRolledback(mImpl->txnId);
    mImpl->commands->brmReleaseTableLock(mImpl->tblLock);
    mImpl->autoRollback = false;
    mImpl->transactionClosed = true;
    summaryImpl->stopTimer();
}

ColumnStoreSummary& ColumnStoreBulkInsert::getSummary()
{
    return *mImpl->summary;
}

void ColumnStoreBulkInsert::setTruncateIsError(bool set)
{
    mImpl->truncateIsError = set;
}

/* Private parts of API below here */

ColumnStoreBulkInsertImpl::ColumnStoreBulkInsertImpl(const std::string& iDb, const std::string& iTable, uint8_t iMode, uint16_t iPm):
    driver(nullptr),
    tbl(nullptr),
    commands(nullptr),
    db(iDb),
    table(iTable),
    mode(iMode),
    pm(iPm),
    uniqueId(0),
    tblLock(0),
    txnId(0),
    row(nullptr),
    batchSize(10000),
    autoRollback(true),
    transactionClosed(false),
    truncateIsError(false),
    currentPm(0)
{
    sessionId = rand() % 65535 + 65535;
    summary = new ColumnStoreSummary();
    if (iMode == 1)
    {
        pmList.push_back(iPm);
    }
}

ColumnStoreBulkInsertImpl::~ColumnStoreBulkInsertImpl()
{
    delete commands;
    delete summary;
}

void ColumnStoreBulkInsertImpl::connect()
{
    commands = new ColumnStoreCommands(driver);
    std::vector<uint32_t> dbRoots;
    if (pmList.size() == 0)
    {
        uint32_t pmCount = driver->getPMCount();
        for (uint32_t pmit = 1; pmit <= pmCount; pmit++)
        {
            pmList.push_back(pmit);
            driver->getDBRootsForPM(pmit, dbRoots);
        }
    }

    if (pmList.size() == 0)
    {
        std::string err("No PMs found in configuration");
        throw ColumnStoreConfigError(err);
    }
    if (dbRoots.size() == 0)
    {
        std::string err("No DBRoots found in configuration");
        throw ColumnStoreConfigError(err);
    }

    if (!commands->procMonCheckVersion())
    {
        std::string err("Incompatible ColumnStore version found");
        throw ColumnStoreVersionError(err);
    }

    tbl = &driver->getSystemCatalog()->getTable(db, table);
    tableData.tableName = tbl->getTableName();
    tableData.tableSchema = tbl->getSchemaName();
    for (uint16_t columnNo = 0; columnNo < tbl->getColumnCount(); columnNo++)
    {
        ColumnStoreSystemCatalogColumn column = tbl->getColumn(columnNo);
        tableData.columns.push_back(ColumnStoreColumnData(column.getOID(), column.getColumnName()));
    }
    txnId = commands->brmGetTxnID(sessionId);
    uniqueId = commands->brmGetUniqueId();
    tblLock = commands->brmGetTableLock(tbl->getOID(), sessionId, txnId, dbRoots);
    for (auto& pmit: pmList)
    {
        commands->weKeepAlive(pmit);
    }
    row = tableData.getRow();
}

}
