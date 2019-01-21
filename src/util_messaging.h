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

class ColumnStoreMessaging
{
public:
    ColumnStoreMessaging() :
        position(0),
        buffer_used(0)
    {
        // Vector won't need to move in RAM for at least 1MB of packet data
        networkData.reserve(1024*1024);
    }
    ColumnStoreMessaging& operator =(const ColumnStoreMessaging &obj);
    ~ColumnStoreMessaging();
    ColumnStoreMessaging& operator <<(const uint8_t data);
    ColumnStoreMessaging& operator <<(const uint16_t data);
    ColumnStoreMessaging& operator <<(const uint32_t data);
    ColumnStoreMessaging& operator <<(const uint64_t data);
    ColumnStoreMessaging& operator <<(const boost::string_ref& data);

    ColumnStoreMessaging& operator >>(uint8_t& data);
    ColumnStoreMessaging& operator >>(uint16_t& data);
    ColumnStoreMessaging& operator >>(uint32_t& data);
    ColumnStoreMessaging& operator >>(uint64_t& data);
    ColumnStoreMessaging& operator >>(std::string& data);

    std::vector<unsigned char>* getDataPtr() { return &networkData; }
    std::vector<size_t>* getLengthsPtr() { return &lengths; }
    // The length of data read from header
    size_t getDataLength();
    bool isCompletePacket();
    bool isCompressedHeader();
    bool isUncompressedHeader();
    uint32_t getHeader() { return HEADER; }
    uint32_t getCompressedHeader() { return COMPRESSED_HEADER; }
    uint8_t getHeaderLength() { return HEADER_LENGTH; }
    size_t getMessageCount() { return lengths.size(); }
    void allocateDataSize(size_t size);
    void clearData()
    {
        networkData.clear();
        buffer_used = 0;
    }

    void setBufferUsedSize(size_t size) { buffer_used = size; }
    unsigned char* getBufferEmptyPos() { return networkData.data()+buffer_used; }
    size_t getBufferUsedSize() { return buffer_used; }
    size_t getBufferFreeSize() { return networkData.size() - buffer_used; }
private:
    // Header bytes used by ByteStream
    const uint32_t HEADER            = 0x14FBC137;
    const uint32_t COMPRESSED_HEADER = 0x14FBC138;
    const uint8_t HEADER_LENGTH = 4;

    // Lengths used for write, position used for read
    std::vector<size_t> lengths;
    std::vector<unsigned char> networkData;
    size_t position;
    size_t buffer_used;

    void addHeader();
};

}
