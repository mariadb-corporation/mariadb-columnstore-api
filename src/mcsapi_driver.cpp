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

namespace mcsapi
{
ColumnStoreDriver::ColumnStoreDriver(const std::string& path)
{
    mImpl = new ColumnStoreDriverImpl();
    mImpl->path = path;
    mImpl->loadXML();
}

ColumnStoreDriver::ColumnStoreDriver()
{
    std::string path(DEFAULT_PATH);
    char* envpath = std::getenv("COLUMNSTORE_INSTALL_DIR");
    if (envpath)
    {
        path = envpath;
        path.append("/etc/Columnstore.xml");
    }


    mImpl = new ColumnStoreDriverImpl();
    mImpl->path = path;
    mImpl->loadXML();
}

ColumnStoreDriver::~ColumnStoreDriver()
{

    delete mImpl;
}

const char* ColumnStoreDriver::getVersion()
{
    const char* version = GIT_VERSION;
    return version;
}

ColumnStoreBulkInsert* ColumnStoreDriver::createBulkInsert(std::string& db,
    std::string& table, uint8_t mode, uint16_t pm)
{
    return new ColumnStoreBulkInsert(this->mImpl, db, table, mode, pm);
}
/* Private parts of API below here */

ColumnStoreDriverImpl::~ColumnStoreDriverImpl()
{
    if (mXmlDoc)
        xmlFreeDoc(mXmlDoc);
}

void ColumnStoreDriverImpl::loadXML()
{
    mXmlDoc = xmlParseFile(path.c_str());
    if (!mXmlDoc)
    {
        throw ColumnStoreDriverException("Error parsing Columnstore XML file " + path);
    }
    mXmlRootNode = xmlDocGetRootElement(mXmlDoc);
    if (!mXmlRootNode)
    {
        throw ColumnStoreDriverException("Could not find the root node of the XML file " + path);
    }
    if (xmlStrcmp(mXmlRootNode->name, (const xmlChar *)"Columnstore"))
    {
        throw ColumnStoreDriverException("The provided XML file is not a Columnstore configuration file " + path);
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
