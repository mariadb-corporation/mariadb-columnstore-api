/* Copyright (c) 2019, MariaDB Corporation. All rights reserved.
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

#include <gtest/gtest.h>
#include "common.h"

MYSQL* get_mariadb()
{
    MYSQL* ma_con;
    std::string cs_ip = "127.0.0.1";
    std::string cs_socket;
    std::string cs_user = "root";
    std::string cs_password = "";
    if(std::getenv("MCSAPI_CS_TEST_IP")){
        cs_ip = std::getenv("MCSAPI_CS_TEST_IP");
    }
    if(std::getenv("MCSAPI_CS_TEST_SOCKET")){
        cs_socket = std::getenv("MCSAPI_CS_TEST_SOCKET");
        cs_ip = "";
    }
    if(std::getenv("MCSAPI_CS_TEST_USER")){
        cs_user = std::getenv("MCSAPI_CS_TEST_USER");
    }
    if(std::getenv("MCSAPI_CS_TEST_PASSWORD")){
        cs_password = std::getenv("MCSAPI_CS_TEST_PASSWORD");
    }
    ma_con = mysql_init(NULL);
    if (!ma_con){
        std::cout << "Could not init MariaDB connection";
        return NULL;
    }
    if (!mysql_real_connect(ma_con, cs_ip.c_str(), cs_user.c_str(), cs_password.c_str(), NULL, 3306, cs_socket.c_str(), 0)){
        std::cout << "Could not connect to MariaDB: " << mysql_error(ma_con);
        return NULL;
    }
    if (mysql_query(ma_con, "CREATE DATABASE IF NOT EXISTS mcsapi")){
        std::cout << "Error creating database: " << mysql_error(ma_con);
        return NULL;
    }
    if (mysql_select_db(ma_con, "mcsapi")){
        std::cout << "Could not select DB: " << mysql_error(ma_con);
        return NULL;
    }

    return ma_con;
}
