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

#include <sstream>

#include "mcsapi_driver_impl.h"
#include "mcsapi_types_impl.h"

namespace mcsapi
{

ColumnStoreCommands::ColumnStoreCommands(ColumnStoreDriverImpl* mcsDriver) :
        brmConnection(nullptr),
        driver(mcsDriver),
        uv_loop(nullptr)
{
    int ret;
    uv_loop = new uv_loop_t;
    ret = uv_loop_init(uv_loop);
    if (ret < 0)
    {
        throw ColumnStoreInternalError("Error setting up internal library");
    }
}

ColumnStoreCommands::~ColumnStoreCommands()
{
    if (uv_loop)
    {
        // Tell anything left over to close
        uv_walk(uv_loop, onCloseWalk, NULL);
        uv_run(uv_loop, UV_RUN_DEFAULT);
    }
    for (auto& it: weConnections)
    {
        delete it.second;
    }

    for (auto& it: miscConnections)
    {
        delete it;
    }
    delete brmConnection;

    uv_loop_close(uv_loop);
    delete uv_loop;
}

void ColumnStoreCommands::onCloseWalk(uv_handle_t* handle, void *arg)
{
    (void) arg;
    uv_close(handle, NULL);
}


int ColumnStoreCommands::runSoloLoop(ColumnStoreNetwork* connection)
{
    int status;
    do
    {
        status = uv_run(uv_loop, UV_RUN_DEFAULT);
        // TODO: Check for status = 0, this means uv_stop() called
        if (status)
        {
        }
    }
    while ((connection->getStatus() != CON_STATUS_CONNECT_ERROR) &&
           (connection->getStatus() != CON_STATUS_IDLE));

    return status;
}

int ColumnStoreCommands::runLoop()
{
    int status;
    bool completed = false;
    do
    {
        status = uv_run(uv_loop, UV_RUN_DEFAULT);
        // TODO: Check for status = 0, this means uv_stop() called
        if (status)
        {
        }
        for (auto& it: weConnections)
        {
            ColumnStoreNetwork* connection = it.second;
            if ((connection->getStatus() == CON_STATUS_CONNECT_ERROR) ||
                (connection->getStatus() == CON_STATUS_IDLE))
            {
                completed = true;
            }
            else
            {
                completed = false;
                break;
            }
        }
    }
    while (completed == false);

    return status;

}

ColumnStoreSystemCatalog* ColumnStoreCommands::brmGetSystemCatalog()
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_GET_SYSTEM_CATALOG;
    messageIn << command;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error getting system catalog");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint32_t table_count;
    *messageOut >> table_count;
    ColumnStoreSystemCatalog* systemCatalog = new ColumnStoreSystemCatalog();
    for (uint32_t i = 0; i < table_count; i++)
    {
        ColumnStoreSystemCatalogTable* table = new ColumnStoreSystemCatalogTable();
        uint32_t column_count;
        *messageOut >> table->mImpl->oid;
        *messageOut >> table->mImpl->schema;
        *messageOut >> table->mImpl->table;
        *messageOut >> column_count;
        mcsdebug("Table: OID: %u, Schema: %s, Table name: %s", table->getOID(), table->getSchemaName().c_str(), table->getTableName().c_str());
        for (uint32_t j = 0; j < column_count; j++)
        {
            ColumnStoreSystemCatalogColumn* column = new ColumnStoreSystemCatalogColumn();
            uint8_t column_type;
            *messageOut >> column->mImpl->oid;
            *messageOut >> column->mImpl->column;
            *messageOut >> column->mImpl->dict_oid;
            *messageOut >> column_type;
            column->mImpl->type = (columnstore_data_types_t) column_type;
            *messageOut >> column->mImpl->width;
            *messageOut >> column->mImpl->position;
            *messageOut >> column->mImpl->default_val;
            *messageOut >> column->mImpl->autoincrement;
            *messageOut >> column->mImpl->precision;
            *messageOut >> column->mImpl->scale;
            *messageOut >> column->mImpl->null;
            *messageOut >> column->mImpl->compression;
            mcsdebug("Column: OID: %u, Name: %s, Dict: %u, Type: %u, Width: %u, Position: %u, Default: %s, Autoinc: %u, Precision: %u, Scale: %u, Not NULL: %u, Compression: %u",
                column->mImpl->oid, column->mImpl->column.c_str(), column->mImpl->dict_oid, column_type, column->mImpl->width, column->mImpl->position,
                column->mImpl->default_val.c_str(), column->mImpl->autoincrement, column->mImpl->precision, column->mImpl->scale, column->mImpl->null, column->mImpl->compression);
            table->mImpl->columns.push_back(column);
        }
        systemCatalog->mImpl->tables.push_back(table);
    }
    delete messageOut;
    return systemCatalog;
}

uint32_t ColumnStoreCommands::brmGetTxnID(uint32_t sessionId)
{
    ColumnStoreTxnID txnId;
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_GET_TXN_ID;
    uint8_t block = 1;
    uint8_t isDDL = 0;
    messageIn << command;
    messageIn << sessionId;
    messageIn << block;
    messageIn << isDDL;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error getting transaction ID");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint8_t isValid;
    *messageOut >> txnId.id;
    *messageOut >> isValid;
    txnId.isValid = txnId.isValid;

    delete messageOut;
    return txnId.id;
}

uint64_t ColumnStoreCommands::brmGetTableLock(uint32_t tableOID, uint32_t sessionId, uint32_t txnId, std::vector<uint32_t>& dbRoots)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_GET_TABLE_LOCK;
    uint64_t id = 0;
    pid_t PID = getpid();
    time_t now = time(NULL);
    std::string ownerName("mcsapi");
    messageIn << command;
    messageIn << id;
    messageIn << tableOID;
    messageIn << ownerName;
    messageIn << (uint32_t)PID;
    messageIn << sessionId;
    messageIn << txnId;
    uint8_t state = LOCK_TYPE_LOADING;
    messageIn << state;
    messageIn << (uint64_t) now;
    messageIn << (uint64_t) dbRoots.size();
    for (std::vector<uint32_t>::iterator it = dbRoots.begin() ; it != dbRoots.end(); ++it)
    {
        messageIn << *it;
    }

    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error getting table lock");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint64_t ret;
    *messageOut >> ret;
    if (ret)
    {
        delete messageOut;
        return ret;
    }
    ColumnStoreTableLock tblLock;
    *messageOut >> tblLock.ownerPID;
    *messageOut >> tblLock.ownerName;
    *messageOut >> tblLock.sessionID;
    *messageOut >> tblLock.ownerTxnID;

    std::stringstream errmsg;

    errmsg << "Table already locked by PID: " << tblLock.ownerPID;
    errmsg << " '" << tblLock.ownerName << "'";
    errmsg << " session ID: " << tblLock.sessionID;
    errmsg << " txn ID: " << tblLock.ownerTxnID;
    delete messageOut;
    throw ColumnStoreServerError(errmsg.str());

    delete messageOut;
    return 0;
}

ColumnStoreNetwork* ColumnStoreCommands::getBrmConnection()
{
    if (brmConnection)
        return brmConnection;

    const char* hostname = driver->getXMLNode("DBRM_Controller", "IPAddr");
    std::string host = hostname;
    brmConnection = new ColumnStoreNetwork(uv_loop, host, PORT_DBRMCONTROLLER);
    return brmConnection;
}

ColumnStoreNetwork* ColumnStoreCommands::getWeConnection(uint32_t pm)
{
    ColumnStoreNetwork* connection;

    try
    {
        connection = weConnections.at(pm);
    }
    catch(...)
    {
        char node_type[32];
        snprintf(node_type, 32, "pm%u_WriteEngineServer", pm);
        const char* hostname = driver->getXMLNode(node_type, "IPAddr");
        std::string host = hostname;
        connection = new ColumnStoreNetwork(uv_loop, host, PORT_WRITEENGINE);
        weConnections[pm] = connection;
    }
    return connection;
}

void ColumnStoreCommands::weBulkRollback(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint64_t tableLockID, uint32_t tableOid)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_BATCH_ROLLBACK;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << sessionId;
    messageIn << tableLockID;
    messageIn << tableOid;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    uint8_t response;
    *messageOut >> uniqueId;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg;
        *messageOut >> errmsg;
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    delete messageOut;
}

void ColumnStoreCommands::weBulkCommit(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId, uint32_t tableOid, std::vector<ColumnStoreHWM>& hwms)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_BATCH_COMMIT;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << txnId;
    messageIn << tableOid;
    messageIn << sessionId;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    uint8_t response;
    *messageOut >> uniqueId;
    *messageOut >> response;
    std::string errmsg;
    *messageOut >> errmsg;
    if (response != 0)
    {
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint64_t bulk_hwm_count;
    *messageOut >> bulk_hwm_count;
    uint32_t oid;
    uint32_t partNum;
    uint32_t segNum; // Note: this is a uint16_t in a struct which gets 4 byte aligned
    uint32_t hwm;
    for (uint64_t i=0; i < bulk_hwm_count; i++)
    {
        *messageOut >> oid;
        *messageOut >> partNum;
        *messageOut >> segNum;
        *messageOut >> hwm;
        hwms.push_back(ColumnStoreHWM(oid, partNum, segNum, hwm));
    }
    delete messageOut;
}

void ColumnStoreCommands::weGetWrittenLbids(uint32_t pm, uint64_t uniqueId, uint32_t txnId, std::vector<uint64_t>& lbids)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_GET_WRITTEN_LBIDS;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << txnId;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    uint8_t response;
    *messageOut >> uniqueId;
    *messageOut >> response;
    std::string errmsg;
    *messageOut >> errmsg;
    if (response != 0)
    {
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint64_t lbid_count;
    *messageOut >> lbid_count;
    uint64_t lbid;
    for (uint64_t i=0; i < lbid_count; i++)
    {
        *messageOut >> lbid;
        lbids.push_back(lbid);
    }
    delete messageOut;
}

void ColumnStoreCommands::weKeepAlive(uint32_t pm)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_KEEPALIVE;
    uint64_t moduleId = 1;
    messageIn << command;
    messageIn << moduleId;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    uint8_t response;
    // 12 bytes of junk
    uint32_t junk1;
    uint64_t junk2;
    *messageOut >> response;
    *messageOut >> junk1;
    *messageOut >> junk2;
    if (response != RESPONSE_OK)
    {
        std::string errmsg("Error attempting to set KeepAlive");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    delete messageOut;
}

void ColumnStoreCommands::weClose(uint32_t pm)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_CLOSE;
    messageIn << command;
    connection->sendData(messageIn);
    runLoop();
    weConnections.erase(pm);
    // Move to miscConnections for cleanup
    miscConnections.push_back(connection);
}


void ColumnStoreCommands::weBulkInsert(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId, ColumnStoreTableData* table)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_BATCH_INSERT_BINARY;
    uint8_t isAutoCommit = 1;
    // InsertSelect mode starts at the next fully available block instead of
    // continuing the current one. When combined with isAutoCommit it avoids
    // using the version buffer.
    uint8_t isInsertSelect = 1;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << txnId;
    messageIn << pm;
    messageIn << sessionId;
    messageIn << isAutoCommit;
    messageIn << isInsertSelect;
    messageIn << table->tableName;
    messageIn << table->tableSchema;
    messageIn << (uint32_t)table->columns.size();
    for (std::vector<ColumnStoreColumnData>::iterator it = table->columns.begin();
         it != table->columns.end(); ++it)
    {
        messageIn << (*it).oid;
        messageIn << (*it).columnName;
    }
    messageIn << (uint32_t)table->row_number;
    for (uint32_t current_column = 0; current_column < table->columns.size(); current_column++)
    {
        for (size_t current_row = 0; current_row < table->row_number; current_row++)
        {
            ColumnStoreDataContainer* data = &table->rows[current_row][current_column];
            uint8_t isNull = data->isNull;
            messageIn << isNull;

            switch (data->datatype)
            {
                case ColumnStoreDataContainer::BYTE:
                    messageIn << data->byte;
                    break;
                case ColumnStoreDataContainer::DUOBYTE:
                    messageIn << data->duobyte;
                    break;
                case ColumnStoreDataContainer::QUADBYTE:
                    messageIn << data->quadbyte;
                    break;
                case ColumnStoreDataContainer::OCTBYTE:
                    messageIn << data->octbyte;
                    break;
                case ColumnStoreDataContainer::VARBYTE:
                    messageIn << data->varbyte;
                    break;
            }
        }

    }

    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    *messageOut >> uniqueId;
    uint8_t response;
    std::string errMsg;
    *messageOut >> response;
    *messageOut >> errMsg;
    if (response != 0)
    {
        std::string errmsg;
        *messageOut >> errmsg;
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::weBulkInsertEnd(uint32_t pm, uint64_t uniqueId, uint32_t txnId, uint32_t tableOid, uint8_t errCode)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_BATCH_INSERT_END;
    uint8_t isAutoCommit = 1;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << txnId;
    messageIn << isAutoCommit;
    messageIn << tableOid;
    messageIn << errCode; // 0 for now
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    *messageOut >> uniqueId;
    uint8_t response;
    std::string errMsg;
    *messageOut >> response;
    *messageOut >> errMsg;
    if (response != 0)
    {
        std::string errmsg;
        *messageOut >> errmsg;
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    delete messageOut;
}

void ColumnStoreCommands::weRollbackBlocks(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_ROLLBACK_BLOCKS;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << sessionId;
    messageIn << txnId;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    *messageOut >> uniqueId;
    uint8_t response;
    std::string errMsg;
    *messageOut >> response;
    *messageOut >> errMsg;
    if (response != 0)
    {
        std::string errmsg;
        *messageOut >> errmsg;
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    delete messageOut;
}

void ColumnStoreCommands::weRemoveMeta(uint32_t pm, uint64_t uniqueId, uint32_t tableOid)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getWeConnection(pm);
    runLoop();

    uint8_t command = COMMAND_WRITEENGINE_BATCH_AUTOON_REMOVE_META;
    messageIn << command;
    messageIn << uniqueId;
    messageIn << tableOid;
    connection->sendData(messageIn);
    runLoop();

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runLoop();

    *messageOut >> uniqueId;
    uint8_t response;
    std::string errMsg;
    *messageOut >> response;
    *messageOut >> errMsg;
    if (response != 0)
    {
        std::string errmsg;
        *messageOut >> errmsg;
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    delete messageOut;
}

uint64_t ColumnStoreCommands::brmGetUniqueId()
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_GET_UNIQUE_ID;
    messageIn << command;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error getting a unique ID");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    uint64_t uniqueId;
    *messageOut >> uniqueId;

    delete messageOut;
    return uniqueId;
}

bool ColumnStoreCommands::procMonCheckVersion()
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    const char* hostname = driver->getXMLNode("pm1_ProcessMonitor", "IPAddr");
    std::string host = hostname;
    ColumnStoreNetwork* connection = new ColumnStoreNetwork(uv_loop, host, PORT_PROCMON);
    miscConnections.push_back(connection);
    // Connect
    runSoloLoop(connection);

    uint8_t command = COMMAND_PROCMON_GET_SOFTWARE_INFO;
    messageIn << command;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    int major, minor, revision;
    std::string version;
    uint8_t response;

    *messageOut >> response;
    if (response != RESPONSE_OK)
    {
        return false;
    }

    *messageOut >> command;
    if (command != COMMAND_PROCMON_GET_SOFTWARE_INFO)
    {
        return false;
    }

    *messageOut >> version;
    std::sscanf(version.c_str(), "%d.%d.%d", &major, &minor, &revision);

    if ((major < 1) || (minor < 1))
    {
        return false;
    }

    delete messageOut;
    return true;
}

void ColumnStoreCommands::brmGetUncommittedLbids(uint32_t txnId, std::vector<uint64_t>& lbids)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_GET_UNCOMMITTED_LBIDS;

    messageIn << command;
    messageIn << txnId;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error getting uncommitted LBIDs");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    uint64_t lbidCount;

    *messageOut >> lbidCount;
    for (uint64_t i = 0; i < lbidCount; i++)
    {
        uint64_t lbid;
        *messageOut >> lbid;
        lbids.push_back(lbid);
    }

    delete messageOut;

}

void ColumnStoreCommands::brmSetHWMAndCP(std::vector<ColumnStoreHWM>& hwms, std::vector<uint64_t>& lbids, uint32_t txnId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_BULK_SET_HWM_AND_CP;

    messageIn << command;
    uint64_t tmp64 = 0;
    messageIn << (uint64_t) hwms.size();
    for (auto& it : hwms)
    {
        messageIn << it.oid;
        messageIn << it.partNum;
        messageIn << it.segNum;
        messageIn << it.hwm;
    }
    messageIn << lbids.size();
    for (auto& it : lbids)
    {
        messageIn << it;
        // min int64
        messageIn << (uint64_t) 0x8000000000000000;
        messageIn << (uint64_t) std::numeric_limits<int64_t>::max();
        // int32_t -1 with 4 byte struct padding which is the same as uint64_t
        // max for our purposes;
        messageIn << std::numeric_limits<uint64_t>::max();
    }

    messageIn << tmp64; // Unused vector
    messageIn << txnId;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error setting HWM");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmSetExtentsMaxMin(std::vector<uint64_t>& lbids)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_SET_EXTENTS_MIN_MAX;

    messageIn << command;
    messageIn << lbids.size();
    for (auto& it : lbids)
    {
        messageIn << it;
        // min int64
        messageIn << (uint64_t) 0x8000000000000000;
        messageIn << (uint64_t) std::numeric_limits<int64_t>::max();
        // int32_t -1 with 4 byte struct padding which is the same as uint64_t
        // max for our purposes;
        messageIn << std::numeric_limits<uint64_t>::max();
    }

    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error setting Extents Max/Min");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmRollback(std::vector<uint64_t>& lbids, uint32_t txnId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_ROLLBACK_VB;

    messageIn << command;
    messageIn << txnId;
    messageIn << lbids.size();
    for (auto& it : lbids)
    {
        messageIn << it;
    }

    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error in VB rollback");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

/* Don't think we need this? */
void ColumnStoreCommands::brmVBCommit(uint32_t txnId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_COMMIT_VB;

    messageIn << command;
    messageIn << txnId;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error committing version buffer");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmCommitted(uint32_t txnId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_COMMITTED;

    messageIn << command;
    messageIn << txnId;
    uint8_t valid = 1;
    messageIn << valid;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error committing BRM");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmTakeSnapshot()
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_TAKE_SNAPSHOT;

    messageIn << command;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error taking BRM snapshot");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmChangeState(uint64_t lockId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_CHANGE_LOCK_STATE;

    messageIn << command;
    messageIn << lockId;
    messageIn << (uint32_t) 1;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error changing BRM lock state");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }

    delete messageOut;
}

void ColumnStoreCommands::brmReleaseTableLock(uint64_t lockId)
{
    ColumnStoreMessaging messageIn;
    ColumnStoreMessaging* messageOut;
    ColumnStoreNetwork *connection = getBrmConnection();
    runSoloLoop(connection);

    uint8_t command = COMMAND_DBRM_RELEASE_TABLE_LOCK;

    messageIn << command;
    messageIn << lockId;
    connection->sendData(messageIn);
    runSoloLoop(connection);

    connection->readDataStart();
    messageOut = connection->getReadMessage();
    runSoloLoop(connection);

    uint8_t response;
    *messageOut >> response;
    if (response != 0)
    {
        std::string errmsg("Error releasing table lock");
        delete messageOut;
        throw ColumnStoreServerError(errmsg);
    }
    // Unknown ignored byte
    uint8_t unknown;
    *messageOut >> unknown;

    delete messageOut;
}

}
