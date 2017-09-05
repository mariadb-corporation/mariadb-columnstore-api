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
/* Connections work using a callback system added to the main loop of
 * the class that creats the connection. This means we can have multiple
 * connections handled in the main loop.
 * Phases (assuming success):
 * 1. constructor starts DNS resolver
 * 2. DNS resolver calls onResolved
 * 3. onResolved starts connection
 * 4. connection calls onConnect
 * We are now ready for data, for this we have
 * onReadData callback for when there is data to be read and onWriteData
 * when the buffer has been sent.
 */

namespace mcsapi
{
ColumnStoreNetwork::ColumnStoreNetwork(uv_loop_t* loop,
        std::string& host, columnstore_ports_t port):
            uv_loop(loop),
            buf(nullptr),
            con_status(CON_STATUS_NONE),
            compressedBuffer(nullptr),
            messageOut(nullptr),
            compressedMessageOut(nullptr),
            dataInBuffer(0),
            isLocalhost (false)
{
    int ret;
    char str_port[7] = {'\0'};
    uv_resolver.data = this;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    snprintf(str_port, 6, "%d", port);
    mcsdebug("Class %p resolving %s port %d", (void*)this, host.c_str(), port);
    // Start by resolving, set callback
    ret = uv_getaddrinfo(uv_loop, &uv_resolver,
            ColumnStoreNetwork::onResolved, host.c_str(), str_port, &hints);
    // If we instantly fail to resolve
    if (ret < 0)
    {
        mcsdebug("Class %p instant fail resolving: %s", (void*)this, uv_err_name(ret));
        con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Could not resolve host ");
        err.append(uv_err_name(ret));
        throw ColumnStoreNetworkError(err);
    }
    if (host == "127.0.0.1")
    {
        isLocalhost = true;
    }

    con_status = CON_STATUS_CONNECTING;
}

ColumnStoreNetwork::~ColumnStoreNetwork()
{
}

void ColumnStoreNetwork::onResolved(uv_getaddrinfo_t* resolver,
        int status, struct addrinfo* res)
{
    // Fake a 'this' for a callback
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)resolver->data;
    mcsdebug("Class %p resolver callback", (void*)This);
    if (status < 0)
    {
        mcsdebug("Class %p failed resolving: %s", (void*)This, uv_err_name(status));
        This->con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Could not resolve host: ");
        err.append(uv_err_name(status));
        throw ColumnStoreNetworkError(err);
    }
    char addr[17] = {'\0'};

    mcsdebug("Class %p resolving success", (void*)This);
    uv_ip4_name((struct sockaddr_in*) res->ai_addr, addr, 16);
    uv_tcp_init(This->uv_loop, &This->uv_tcp);
    This->uv_tcp.data = This;
    This->uv_connection.data = (void*)&This->uv_tcp;
    // Make TCP connection
    uv_tcp_connect(&This->uv_connection, &This->uv_tcp, (const struct sockaddr*)
            res->ai_addr, ColumnStoreNetwork::onConnect);

    uv_freeaddrinfo(res);
}

void ColumnStoreNetwork::onConnect(uv_connect_t* req, int status)
{
    // Fake a 'this' for a callback
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)req->handle->data;
    mcsdebug("Class %p connect callback", (void*)This);
    if (status < 0)
    {
        mcsdebug("Class %p connection failure: %s", (void*)This, uv_err_name(status));
        This->con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Connection failure: ");
        err.append(uv_err_name(status));
        throw ColumnStoreNetworkError(err);
    }
    mcsdebug("Class %p connection succeeded", (void*)This);
    This->con_status = CON_STATUS_CONNECTED;
    // Stream is basically the TCP connection details
    This->uv_stream = (uv_stream_t*)req->data;

    int ret = uv_read_start(This->uv_stream, ColumnStoreNetwork::onConnectAlloc,
                            ColumnStoreNetwork::onConnectReadData);
    if (ret < 0)
    {
        mcsdebug("Class %p instant fail reading data: %s", (void*)This, uv_err_name(ret));
        This->con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Could not read data: ");
        err.append(uv_err_name(ret));
        throw ColumnStoreNetworkError(err);
    }

    // "Check" is something that runs once per loop so we can see if there if
    // there is something new to do.
//    uv_check_init(This->uv_loop, &This->uv_check);
//    This->uv_check.data = This;
//    uv_check_start(&This->uv_check, ColumnStoreNetwork::onLoop);
}

void ColumnStoreNetwork::onConnectReadData(uv_stream_t* tcp, ssize_t read_size, const uv_buf_t* buf)
{
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)tcp->data;
    mcsdebug("Class %p read connect callback %zd bytes", (void*)This, read_size);
    uv_read_stop(tcp);

    if (read_size < 0)
    {
        mcsdebug("Class %p fail reading data: %s", (void*)This, uv_err_name(read_size));
        This->con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Could not read data: ");
        err.append(uv_err_name(read_size));
        throw ColumnStoreNetworkError(err);
    }

    if ((read_size != 1) || (buf->base[0] != 'A'))
    {
        delete[] buf->base;
        This->con_status = CON_STATUS_CONNECT_ERROR;
        std::string err("Incorrect read data during handshake");
        throw ColumnStoreNetworkError(err);
    }
    This->con_status = CON_STATUS_IDLE;
    delete[] buf->base;
}

void ColumnStoreNetwork::onConnectAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf)
{
    // Avoid unused variable warning
#if DEBUG
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)client->data;
    mcsdebug("Class %p increasing read connect buffer to %zu bytes", (void*)This, suggested_size);
#endif
    char* buffer = new char[suggested_size];
    buf->base = buffer;
    buf->len = suggested_size;
}


void ColumnStoreNetwork::onLoop(uv_check_t *handle)
{
    (void) handle;
    // Fake a 'this' for a callback
    //ColumnStoreNetwork* This = (ColumnStoreNetwork*)handle->data;
    // Only uncomment this for extreme debugging, it will fire often!!
    // mcsdebug("Class %p loop callback", (void*)This);
}

void ColumnStoreNetwork::onReadData(uv_stream_t* tcp, ssize_t read_size, const uv_buf_t* buf)
{
    (void) buf;
    // Fake a 'this' for a callback
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)tcp->data;
    mcsdebug("Class %p read callback %zd bytes", (void*)This, read_size);
    if (read_size < 0)
    {
        mcsdebug("Class %p fail reading data: %s", (void*)This, uv_err_name(read_size));
        This->con_status = CON_STATUS_NET_ERROR;
        std::string err("Could not read data: ");
        err.append(uv_err_name(read_size));
        throw ColumnStoreNetworkError(err);
    }
    mcsdebug_hex(buf->base, (size_t)read_size);
    This->dataInBuffer += read_size;
    // TODO: need a better way of tracking this
    This->messageOut->setDataSize(This->dataInBuffer);
    if (This->messageOut->isCompletePacket())
    {
        // TODO: move to readDataStop?
        uv_read_stop(tcp);
        This->con_status = CON_STATUS_IDLE;
        if (This->messageOut->isCompressedHeader())
        {
            size_t result_length;
            const char* packet = reinterpret_cast<char*>(This->messageOut->getDataPtr()->data());
            if (snappy::GetUncompressedLength(packet, This->dataInBuffer, &result_length))
            {
                This->uncompressData(result_length);
            }
        }
        else if (!This->messageOut->isUncompressedHeader())
        {
            mcsdebug ("Class %p bad packet from server", (void*)This);
            std::string err("Bad packet from server");
            This->con_status = CON_STATUS_NET_ERROR;
            throw ColumnStoreNetworkError(err);
        }
    }
}

void ColumnStoreNetwork::uncompressData(size_t result_length)
{
    char* uncompressed = nullptr;
    char* compressed = nullptr;
    if (compressedMessageOut)
    {
        delete compressedMessageOut;
    }
    compressedMessageOut = messageOut;
    messageOut = new ColumnStoreMessaging();
    compressed =  reinterpret_cast<char*>(compressedMessageOut->getDataPtr()->data());
    messageOut->allocateDataSize(result_length+8);
    uncompressed = reinterpret_cast<char*>(messageOut->getDataPtr()->data()+8);
    if (!snappy::RawUncompress(compressed, dataInBuffer, uncompressed))
    {
        mcsdebug("Class %p fail decompressing data", (void*)this);
        con_status = CON_STATUS_NET_ERROR;
        std::string err("Compressed data corruption");
        throw ColumnStoreNetworkError(err);
    }
    char* uncompressedHeader = reinterpret_cast<char*>(messageOut->getDataPtr()->data());
    // Add fake decompressed header since we don't get one
    uint32_t headerData = messageOut->getHeader();
    memcpy(uncompressedHeader, &headerData, 4);
    memcpy(uncompressedHeader+4, &result_length, 4);
    messageOut->setDataSize(result_length+8);
}

void ColumnStoreNetwork::onWriteData(uv_write_t* req, int status)
{
    // Fake a 'this' for a callback
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)req->data;
    mcsdebug("Class %p write callback", (void*)This);
    delete req;
    delete[] This->buf;
    This->buf = nullptr;
    delete This->compressedBuffer;
    This->compressedBuffer = nullptr;
    if (status < 0)
    {
        mcsdebug("Class %p write failure: %s", (void*)This, uv_err_name(status));
        This->con_status = CON_STATUS_NET_ERROR;
        std::string err("Write failure: ");
        err.append(uv_err_name(status));
        throw ColumnStoreNetworkError(err);
    }
    This->con_status = CON_STATUS_IDLE;
}

void ColumnStoreNetwork::sendData(ColumnStoreMessaging& message)
{
    std::vector<unsigned char>* packet_data = message.getDataPtr();

    if ((!isLocalhost) && (packet_data->size() > 512))
    {
        sendCompressedData(message);
        return;
    }
    mcsdebug("Class %p sending %zu bytes", this, packet_data->size());
    if (packet_data->size() == 0)
    {
        return;
    }
    buf = new uv_buf_t[message.getMessageCount()+1];

    // Send the 4 byte header and then 4 byte length
    uint32_t dataLength = packet_data->size() - 8;
    char* data = reinterpret_cast<char*>(packet_data->data());
    // Set size part of header
    memcpy(data+4, (char*)&dataLength, 4);
    buf[0].base = data;
    buf[0].len = 8;
    // Send the data
    mcsdebug_hex(data, packet_data->size());
    data+= 8;
    std::vector<size_t>* packet_lengths = message.getLengthsPtr();
    size_t length_count = 0;
    size_t length_total = 0;
    for (std::vector<size_t>::iterator it = packet_lengths->begin();
        it != packet_lengths->end(); ++it)
    {
        // Start at 1 because 0 is header
        length_count++;
        buf[length_count].base = data+length_total;
        buf[length_count].len = *it;
        length_total += *it;
    }
    writeData(message.getMessageCount()+1);
}

void ColumnStoreNetwork::writeData(size_t buffer_count)
{
    int ret;

    // libuv will crash if this is stack allocated, freed in callback
    uv_write_t *req = new uv_write_t;
    req->data = this;
    this->con_status = CON_STATUS_BUSY;

    ret = uv_write(req, this->uv_stream, buf, buffer_count,
                    ColumnStoreNetwork::onWriteData);
    if (ret < 0)
    {
        mcsdebug("Class %p instant fail sending data: %s", (void*)this, uv_err_name(ret));
        con_status = CON_STATUS_NET_ERROR;
        std::string err("Could not send data: ");
        err.append(uv_err_name(ret));
        throw ColumnStoreNetworkError(err);
    }
}

void ColumnStoreNetwork::sendCompressedData(ColumnStoreMessaging& message)
{
    std::vector<unsigned char>* packet_data = message.getDataPtr();
    buf = new uv_buf_t[1];
    compressedBuffer = new char[snappy::MaxCompressedLength(packet_data->size()) + 8];
    size_t compressed_length;
    char* data = reinterpret_cast<char*>(packet_data->data());

    uint32_t header = message.getCompressedHeader();
    memcpy(compressedBuffer, &header, 4);

    char* compressedDataStart = compressedBuffer + 8;
    // Shift 8 bytes as we don't send the uncompressed header
    snappy::RawCompress(data+8, packet_data->size()-8, compressedDataStart, &compressed_length);
    mcsdebug("Class %p sending %zu bytes compressed to %zu bytes", this, packet_data->size(), compressed_length);
    if (packet_data->size() == 0)
    {
        return;
    }

    this->con_status = CON_STATUS_BUSY;

    memcpy(compressedBuffer+4, &compressed_length, 4);
    buf[0].base = compressedBuffer;
    buf[0].len = compressed_length + 8;
    mcsdebug_hex(data, packet_data->size());
    mcsdebug_hex(compressedBuffer, compressed_length);

    // Send the data
    writeData(1);
}

void ColumnStoreNetwork::readDataStart()
{
    int ret;
    this->con_status = CON_STATUS_BUSY;
    dataInBuffer = 0;
    this->messageOut = new ColumnStoreMessaging();
    mcsdebug("Class %p starting read", this);

    ret = uv_read_start(this->uv_stream, ColumnStoreNetwork::onAlloc, ColumnStoreNetwork::onReadData);
    if (ret < 0)
    {
        mcsdebug("Class %p instant fail reading data: %s", (void*)this, uv_err_name(ret));
        con_status = CON_STATUS_NET_ERROR;
        std::string err("Could not read data: ");
        err.append(uv_err_name(ret));
        throw ColumnStoreNetworkError(err);
    }
}

void ColumnStoreNetwork::readDataStop()
{

}

void ColumnStoreNetwork::onAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf)
{
    ColumnStoreNetwork* This = (ColumnStoreNetwork*)client->data;
    mcsdebug("Class %p increasing read buffer to %zu bytes", (void*)This, suggested_size);
    std::vector<unsigned char> *packet_data = This->messageOut->getDataPtr();
    This->messageOut->allocateDataSize(suggested_size);
    buf->base = reinterpret_cast<char*>(packet_data->data());
    buf->len = suggested_size;
}

}
