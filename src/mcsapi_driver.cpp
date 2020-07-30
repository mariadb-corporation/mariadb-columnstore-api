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
#include "version.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "mcsapi_driver_impl.h"
#include "mcsapi_types_impl.h"

namespace mcsapi
{
ColumnStoreDriver::ColumnStoreDriver(const std::string& path)
{
    mcsdebug("ColumnStoreDriver %p constructor called", (void*)this);
    mcsdebug("mcsapi version %s", this->getVersion());
    mImpl = new ColumnStoreDriverImpl();
    mImpl->path = path;
    mImpl->loadXML();
    mcsdebug("loaded config: %s", path.c_str());
    timeval t1;
    gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
}

ColumnStoreDriver::ColumnStoreDriver()
{
    mcsdebug("ColumnStoreDriver %p constructor called", (void*)this);
    mcsdebug("mcsapi version %s", this->getVersion());
    mImpl = new ColumnStoreDriverImpl();
    mImpl->path = "/etc/columnstore/Columnstore.xml";

    mImpl->loadXML();
    mcsdebug("loaded config: %s", mImpl->path.c_str());
    timeval t1;
    gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
}

ColumnStoreDriver::~ColumnStoreDriver()
{
    mcsdebug("ColumnStoreDriver %p deconstructor called", (void*)this);
    delete mImpl;
}

const char* ColumnStoreDriver::getVersion()
{
    const char* version = GIT_VERSION;
    return version;
}

void ColumnStoreDriver::setDebug(uint8_t level)
{
    mcsdebug_set(level);
    mcsdebug("mcsapi debugging set to level %d, version %s", level, this->getVersion());
}

std::vector<TableLockInfo> ColumnStoreDriver::listTableLocks()
{
    ColumnStoreCommands* commands = new ColumnStoreCommands(this->mImpl);
    std::vector<TableLockInfo> tableLocks;
    commands->brmGetAllTableLocks(tableLocks);
    delete commands;
    return tableLocks;
}

bool ColumnStoreDriver::isTableLocked(const std::string& db, const std::string& table, TableLockInfo& rtn)
{
    uint32_t oid = this->getSystemCatalog().getTable(db,table).getOID();
    std::vector<TableLockInfo> tableLocks = this->listTableLocks();
    for (auto& tableLock : tableLocks){
        if (tableLock.tableOID == oid) {
            rtn = tableLock;
            return true;
        }
    }

    return false;
}

bool ColumnStoreDriver::isTableLocked(const std::string& db, const std::string& table) {
    mcsapi::TableLockInfo tbi;
    return(this->isTableLocked(db, table, tbi));
}

void ColumnStoreDriver::clearTableLock(uint64_t lockId) {
    ColumnStoreCommands* commands = new ColumnStoreCommands(this->mImpl);

    // grab the lock to clear and delete it
    TableLockInfo tli = commands->brmGetTableLockInfo(lockId);
    clearTableLock(tli);

    delete commands;
}

void ColumnStoreDriver::clearTableLock(TableLockInfo tbi) 
{
    ColumnStoreCommands* commands = new ColumnStoreCommands(this->mImpl);

    // get the list of PMs to restore this transaction 
    // (all PMs not only the ones assisiated to the mentioned dbroots just in case a dbroot has been moved to a different PM)
    std::vector<uint16_t> pmList;
    std::vector<uint32_t> dbRoots;
    if (pmList.size() == 0)
    {
        uint32_t pmCount = this->mImpl->getPMCount();
        for (uint32_t pmit = 1; pmit <= pmCount; pmit++)
        {
            pmList.push_back(pmit);
            this->mImpl->getDBRootsForPM(pmit, dbRoots);
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
/*
    if (!commands->procMonCheckVersion())
    {
        std::string err("Incompatible ColumnStore version found");
        throw ColumnStoreVersionError(err);
    }
*/
    // get a connection
    for (auto& pmit : pmList)
    {
        commands->weKeepAlive(pmit);
    }

    // send rollback msg to writeEngine server for every PM
    uint64_t uniqueId = commands->brmGetUniqueId();
    for (auto& pmit : pmList)
    {
        std::vector<uint64_t> lbids;
        commands->weGetWrittenLbids(pmit, uniqueId, tbi.ownerTxnID, lbids);
        commands->weRollbackBlocks(pmit, uniqueId, tbi.ownerSessionID, tbi.ownerTxnID);
        commands->brmRollback(lbids, tbi.ownerTxnID);
        commands->weBulkRollback(pmit, uniqueId, tbi.ownerSessionID, tbi.id, tbi.tableOID);
    }

    // change lock state to cleanup
    commands->brmChangeState(tbi.id);

    // delete metadata backup rollback files
    for (auto& pmit : pmList)
    {
        commands->weRemoveMeta(pmit, uniqueId, tbi.tableOID);
        commands->weClose(pmit);
    }
    commands->brmRolledback(tbi.ownerTxnID);

    // release table lock
    commands->brmReleaseTableLock(tbi.id);
    
    delete commands;
}

void ColumnStoreDriver::clearTableLock(const std::string& db, const std::string& table)
{
    uint32_t oid = this->getSystemCatalog().getTable(db, table).getOID();
    std::vector<TableLockInfo> tableLocks = this->listTableLocks();
    for (auto& tableLock : tableLocks) {
        if (tableLock.tableOID == oid) {
            clearTableLock(tableLock);
        }
    }
}

ColumnStoreBulkInsert* ColumnStoreDriver::createBulkInsert(const std::string& db,
    const std::string& table, uint8_t mode, uint16_t pm)
{
    return new ColumnStoreBulkInsert(this->mImpl, db, table, mode, pm);
}

ColumnStoreSystemCatalog& ColumnStoreDriver::getSystemCatalog()
{
    return *mImpl->getSystemCatalog();
}

/* Private parts of API below here */

ColumnStoreSystemCatalog* ColumnStoreDriverImpl::getSystemCatalog()
{
    if (systemCatalog)
    {
        return systemCatalog;
    }
    ColumnStoreCommands* commands = new ColumnStoreCommands(this);
    systemCatalog = commands->brmGetSystemCatalog();
    if (!systemCatalog)
    {
        std::string err("Empty system catalog retrieved");
        throw ColumnStoreServerError(err);
    }
    delete commands;
    return systemCatalog;
}

ColumnStoreDriverImpl::~ColumnStoreDriverImpl()
{
    if (systemCatalog && systemCatalog->mImpl)
    {
        systemCatalog->mImpl->clear();
    }
    delete systemCatalog;
    if (mXmlDoc)
        xmlFreeDoc(mXmlDoc);
}

void ColumnStoreDriverImpl::loadXML()
{
    mXmlDoc = xmlParseFile(path.c_str());
    if (!mXmlDoc)
    {
        throw ColumnStoreConfigError("Error parsing Columnstore XML file " + path);
    }
    mXmlRootNode = xmlDocGetRootElement(mXmlDoc);
    if (!mXmlRootNode)
    {
        throw ColumnStoreConfigError("Could not find the root node of the XML file " + path);
    }
    if (xmlStrcmp(mXmlRootNode->name, (const xmlChar *)"Columnstore"))
    {
        throw ColumnStoreConfigError("The provided XML file is not a Columnstore configuration file " + path);
    }
}

const char* ColumnStoreDriverImpl::getXMLNode(const char* parent, const char* node)
{
    xmlNodePtr xmlParentNode = mXmlRootNode->xmlChildrenNode;
    while (xmlParentNode != NULL)
    {
        if (!xmlStrcmp(xmlParentNode->name, (const xmlChar *)parent))
        {
            xmlNodePtr xmlChildNode = xmlParentNode->xmlChildrenNode;
            while (xmlChildNode != NULL)
            {
                if (!xmlStrcmp(xmlChildNode->name, (const xmlChar *)node))
                {
                    xmlNodePtr xmlRetNode = xmlChildNode->xmlChildrenNode;
                    if (xmlRetNode)
                        return (const char*)xmlRetNode->content;
                    return NULL;
                }
                xmlChildNode = xmlChildNode->next;
            }
        }
        xmlParentNode = xmlParentNode->next;
    }
    return NULL;
}

uint32_t ColumnStoreDriverImpl::getXMLNodeUint(const char* parent, const char* node)
{
    const char* xmlRtnString = getXMLNode(parent, node);
    if (xmlRtnString == NULL){
        return 0;
    }
    uint32_t rtn = strtoul(xmlRtnString, NULL, 10);

    return rtn;
}

uint32_t ColumnStoreDriverImpl::getPMCount()
{
    const char* pmStringCount = getXMLNode("PrimitiveServers", "Count");
    uint32_t pmCount = strtoul(pmStringCount, NULL, 10);

    return pmCount;
}

uint32_t ColumnStoreDriverImpl::getDBRootCount()
{
    const char* dbRootStringCount = getXMLNode("SystemConfig", "DBRootCount");
    uint32_t dbRootCount = strtoul(dbRootStringCount, NULL, 10);

    return dbRootCount;
}

void ColumnStoreDriverImpl::getDBRootsForPM(uint32_t pm, std::vector<uint32_t>& dbRoots)
{
    char dbRootXMLName[32];
    snprintf(dbRootXMLName, 32, "ModuleDBRootCount%" PRIu32 "-3", pm);
    const char* dbRootStringCount = getXMLNode("SystemModuleConfig", dbRootXMLName);
    uint32_t dbRootCount = strtoul(dbRootStringCount, NULL, 10);
    for (uint32_t dbRC = 1; dbRC <= dbRootCount; dbRC++)
    {
        snprintf(dbRootXMLName, 32, "ModuleDBRootID%" PRIu32 "-%" PRIu32 "-3", pm, dbRC);
        const char* dbRootStringID = getXMLNode("SystemModuleConfig", dbRootXMLName);
        uint32_t dbRootID = strtoul(dbRootStringID, NULL, 10);
        dbRoots.push_back(dbRootID);
    }
}

}
