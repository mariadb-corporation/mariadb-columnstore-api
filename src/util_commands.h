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

enum columnstore_commands_t
{
    COMMAND_PROCMON_GET_SOFTWARE_INFO = 0x0E,
    COMMAND_DBRM_ROLLBACK_VB = 0x07,
    COMMAND_DBRM_COMMIT_VB = 0x08,
    COMMAND_DBRM_SET_EXTENTS_MIN_MAX = 0x1A,
    COMMAND_DBRM_TAKE_SNAPSHOT = 0x1F,
    COMMAND_DBRM_BULK_SET_HWM_AND_CP = 0x28,
    COMMAND_DBRM_GET_TXN_ID = 0x2E,
    COMMAND_DBRM_COMMITTED = 0x2F,
    COMMAND_DBRM_ROLLEDBACK = 0x30,
    COMMAND_DBRM_GET_UNIQUE_ID = 0x38,
    COMMAND_DBRM_GET_TABLE_LOCK = 0x46,
    COMMAND_DBRM_RELEASE_TABLE_LOCK = 0x47,
    COMMAND_DBRM_CHANGE_LOCK_STATE = 0x48,
	COMMAND_DBRM_GET_ALL_TABLE_LOCKS = 0x4A,
	COMMAND_DBRM_GET_TABLE_LOCK_INFO = 0x4C,
    COMMAND_DBRM_GET_UNCOMMITTED_LBIDS = 0x3A,
    COMMAND_DBRM_GET_SYSTEM_CATALOG = 0x65,
    COMMAND_WRITEENGINE_KEEPALIVE = 0x02,
    COMMAND_WRITEENGINE_ROLLBACK_BLOCKS = 0x08,
    COMMAND_WRITEENGINE_BATCH_INSERT = 0x1B,
    COMMAND_WRITEENGINE_BATCH_INSERT_BINARY = 0x37,
    COMMAND_WRITEENGINE_BATCH_INSERT_END = 0x1C,
    COMMAND_WRITEENGINE_BATCH_COMMIT = 0x1D,
    COMMAND_WRITEENGINE_BATCH_ROLLBACK = 0x1E,
    COMMAND_WRITEENGINE_BATCH_AUTOON_REMOVE_META = 0x21,
    COMMAND_WRITEENGINE_CLOSE = 0x30,
    COMMAND_WRITEENGINE_GET_WRITTEN_LBIDS = 0x38
};

enum columnstore_responses_t
{
    RESPONSE_OK = 0x01
};

class ColumnStoreCommands
{
public:
    ColumnStoreCommands(ColumnStoreDriverImpl* mcsDriver);
    ~ColumnStoreCommands();

    bool procMonCheckVersion();
    ColumnStoreSystemCatalog* brmGetSystemCatalog();
    uint32_t brmGetTxnID(uint32_t sessionId);
    uint64_t brmGetTableLock(uint32_t tableOID, uint32_t sessionId, uint32_t txnId, std::vector<uint32_t>& dbRoots);
    uint64_t brmGetUniqueId();
    void brmSetHWMAndCP(std::vector<ColumnStoreHWM>& hwms, std::vector<uint64_t>& lbids, uint32_t txnId);
    void brmVBCommit(uint32_t txnId);
    void brmCommitted(uint32_t txnId);
    void brmRolledback(uint32_t txnId);
    void brmReleaseTableLock(uint64_t lockId);
    void brmGetUncommittedLbids(uint32_t txnId, std::vector<uint64_t>& lbids);
    void brmTakeSnapshot();
    void brmChangeState(uint64_t lockId);
    void brmRollback(std::vector<uint64_t>& lbids, uint32_t txnId);
    void brmSetExtentsMaxMin(std::vector<uint64_t>& lbids);
	std::vector<TableLockInfo> brmGetAllTableLocks();
    void weBulkCommit(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId, uint32_t tableOid, std::vector<ColumnStoreHWM>& hwms);
    void weBulkRollback(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint64_t tableLockID, uint32_t tableOid);
    void weBulkInsert(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId, ColumnStoreTableData* table);
    void weBulkInsertEnd(uint32_t pm, uint64_t uniqueId, uint32_t txnId, uint32_t tableOid, uint8_t errCode);
    void weKeepAlive(uint32_t pm);
    void weClose(uint32_t pm);
    void weRemoveMeta(uint32_t pm, uint64_t uniqueId, uint32_t tableOid);
    void weGetWrittenLbids(uint32_t pm, uint64_t uniqueId, uint32_t txnId, std::vector<uint64_t>& lbids);
    void weRollbackBlocks(uint32_t pm, uint64_t uniqueId, uint32_t sessionId, uint32_t txnId);

    static void onCloseWalk(uv_handle_t* handle, void *arg);
private:
    std::map<uint32_t, ColumnStoreNetwork*> weConnections;
    ColumnStoreNetwork* brmConnection;
    std::vector<ColumnStoreNetwork*> miscConnections;
    ColumnStoreDriverImpl* driver;
    uv_loop_t* uv_loop;

    int runLoop();
    int runSoloLoop(ColumnStoreNetwork* connection);
    ColumnStoreNetwork* getWeConnection(uint32_t pm);
    ColumnStoreNetwork* getBrmConnection();
};


}
