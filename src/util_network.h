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

enum columnstore_ports_t
{
    PORT_EXEMGR = 8601,
    PORT_PRIMPROC = 8620,
    PORT_WRITEENGINE = 8630,
    PORT_DBRMCONTROLLER = 8616,
    PORT_DBRMWORKER = 8700,
    PORT_PROCMON = 8800
};

enum columnstore_con_status_t
{
    CON_STATUS_NONE,
    CON_STATUS_IDLE,
    CON_STATUS_NOT_CONNECTED,
    CON_STATUS_CONNECTING,
    CON_STATUS_CONNECT_ERROR,
    CON_STATUS_CONNECTED,
    CON_STATUS_BUSY,
    CON_STATUS_NET_ERROR
};

class ColumnStoreNetwork
{
public:
    ColumnStoreNetwork(uv_loop_t* loop, std::string& host, columnstore_ports_t port);

    columnstore_con_status_t getStatus() { return con_status; };
    void sendData(ColumnStoreMessaging& message);
    void readDataStart();
    void readDataStop();
    ColumnStoreMessaging* getReadMessage() { return messageOut; };
    void deleteReadMessage() { delete messageOut; messageOut = nullptr; };
    size_t getDataInBuffer() { return dataInBuffer; };
    void uncompressData(size_t result_length);
    const std::string& getErrMsg() { return errMsg; };

    ~ColumnStoreNetwork();
private:
    uv_loop_t* uv_loop;
    uv_tcp_t uv_tcp;
    uv_stream_t* uv_stream;
    uv_buf_t* buf;
    uv_connect_t uv_connection;
    //uv_check_t uv_check;
    uv_getaddrinfo_t uv_resolver;
    struct addrinfo hints;

    columnstore_con_status_t con_status;
    char* compressedBuffer;
    ColumnStoreMessaging* messageOut;
    ColumnStoreMessaging* compressedMessageOut;
    size_t dataInBuffer;
    bool isLocalhost;
    std::string errMsg;

    void writeData(size_t buffer_count);
    void sendCompressedData(ColumnStoreMessaging& message);

public:
    static void onConnect(uv_connect_t* req, int status);
    static void onResolved(uv_getaddrinfo_t* resolver, int status, struct addrinfo* res);
    static void onReadData(uv_stream_t* tcp, ssize_t read_size, const uv_buf_t* buf);
    static void onWriteData(uv_write_t* req, int status);
    static void onLoop(uv_check_t *handle);
    static void onAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf);
    static void onConnectReadData(uv_stream_t* tcp, ssize_t read_size, const uv_buf_t* buf);
    static void onConnectAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf);
};

}
