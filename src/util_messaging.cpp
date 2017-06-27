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

namespace mcsapi
{

ColumnStoreMessaging::~ColumnStoreMessaging()
{
    // TODO: Maybe don't need this now?
}
ColumnStoreMessaging& ColumnStoreMessaging::operator <<(const uint8_t data)
{
    addHeader();
    networkData.insert(networkData.end(), (unsigned char*)&data, (unsigned char*)&data + 1);
    lengths.push_back(1);

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator <<(const uint16_t data)
{
    addHeader();
    networkData.insert(networkData.end(), (unsigned char*)&data, (unsigned char*)&data + 2);
    lengths.push_back(2);

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator <<(const uint32_t data)
{
    addHeader();
    networkData.insert(networkData.end(), (unsigned char*)&data, (unsigned char*)&data + 4);
    lengths.push_back(4);

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator <<(const uint64_t data)
{
    addHeader();
    networkData.insert(networkData.end(), (unsigned char*)&data, (unsigned char*)&data + 8);
    lengths.push_back(8);

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator <<(const std::string& data)
{
    addHeader();
    // 4 bytes for length, then data
    uint32_t length = data.length();
    networkData.insert(networkData.end(), (unsigned char*)&length, (unsigned char*)&length + 4);
    lengths.push_back(4);
    networkData.insert(networkData.end(), (unsigned char*)data.c_str(), (unsigned char*)data.c_str() + data.length());
    lengths.push_back(data.length());

    return *this;
}

void ColumnStoreMessaging::addHeader()
{
    if (networkData.size() == 0)
    {
        networkData.insert(networkData.end(), (unsigned char*)&HEADER, ((unsigned char*)&HEADER)+4);
        networkData.insert(networkData.end(), 4, '\0');
    }
}

ColumnStoreMessaging& ColumnStoreMessaging::operator >>(uint8_t& data)
{
    if (position == 0)
    {
        position += 8;
    }

    if (position+1 <= current_size)
    {
        data = networkData[position];
        position++;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator >>(uint16_t& data)
{
    if (position == 0)
    {
        position += 8;
    }

    if (position+2 <= current_size)
    {
        memcpy(&data, &(networkData[position]), 2);
        position += 2;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator >>(uint32_t& data)
{
    if (position == 0)
    {
        position += 8;
    }

    if (position+4 <= current_size)
    {
        memcpy(&data, &(networkData[position]), 4);
        position += 4;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator >>(uint64_t& data)
{
    if (position == 0)
    {
        position += 8;
    }

    if (position+8 <= current_size)
    {
        memcpy(&data, &(networkData[position]), 8);
        position += 8;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    return *this;
}
ColumnStoreMessaging& ColumnStoreMessaging::operator >>(std::string& data)
{
    if (position == 0)
    {
        position += 8;
    }

    // 4 bytes for length, then data
    uint32_t length;
    if (position+4 <= current_size)
    {
        memcpy(&length, &(networkData[position]), 4);
        position += 4;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    if (position+length <= current_size)
    {
        data.assign((const char*)&(networkData[position]), (size_t)length);
        position += length;
    }
    else
    {
        std::string err("Message buffer read past end");
        throw ColumnStoreException(err);
    }

    return *this;
}

size_t ColumnStoreMessaging::getDataLength()
{
    uint32_t length;
    // Not long enough to calculate length
    if (networkData.size() < 8)
    {
        return 0;
    }
    memcpy(&length, &(networkData[4]), 4);

    return length;
}

bool ColumnStoreMessaging::isCompletePacket()
{
    uint32_t length = getDataLength();
    if ((current_size > 8) && (current_size == length + 8))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ColumnStoreMessaging::isCompressedHeader()
{
    if (!isCompletePacket())
    {
        return false;
    }
    if (!memcmp(&networkData[0], &COMPRESSED_HEADER, 4))
    {
        return true;
    }
    return false;
}

bool ColumnStoreMessaging::isUncompressedHeader()
{
    if (!isCompletePacket())
    {
        return false;
    }
    if (!memcmp(&networkData[0], &HEADER, 4))
    {
        return true;
    }
    return false;
}

}
