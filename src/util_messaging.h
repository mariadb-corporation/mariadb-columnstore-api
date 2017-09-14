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
        current_size(0)
    {
        networkData.reserve(65536);
    }
    ColumnStoreMessaging& operator =(const ColumnStoreMessaging &obj);
    ~ColumnStoreMessaging();
    ColumnStoreMessaging& operator <<(const uint8_t data);
    ColumnStoreMessaging& operator <<(const uint16_t data);
    ColumnStoreMessaging& operator <<(const uint32_t data);
    ColumnStoreMessaging& operator <<(const uint64_t data);
    ColumnStoreMessaging& operator <<(const std::string& data);

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
    void allocateDataSize(size_t size) { networkData.resize(size+current_size); }
    void clearData() { networkData.clear(); }
    void setDataSize(size_t size) { current_size = size; }
private:
    // Header bytes used by ByteStream
    const uint32_t HEADER            = 0x14FBC137;
    const uint32_t COMPRESSED_HEADER = 0x14FBC138;
    const uint8_t HEADER_LENGTH = 4;

    // Lengths used for write, position used for read
    std::vector<size_t> lengths;
    std::vector<unsigned char> networkData;
    size_t position;

    size_t current_size;

    void addHeader();
};

}
