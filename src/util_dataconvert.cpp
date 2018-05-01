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
#include "mcsapi_types_impl.h"

namespace mcsapi
{

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, uint64_t fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    uint8_t val8;
    uint16_t val16;
    uint32_t val32;
    uint64_t val64;
    float valF;
    double valD;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            if (fromValue > 1)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = 1;
            }
            else
            {
                val8 = fromValue;
            }
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            if (fromValue > INT8_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MAX;
            }
            else
            {
                val8 = fromValue;
            }
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            if (fromValue > INT16_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MAX;
            }
            else
            {
                val16 = fromValue;
            }
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        case DATA_TYPE_DECIMAL:
            val64 = (uint64_t)(fromValue * pow((double)10, toMeta->getScale()));
            switch (toMeta->getWidth())
            {
                case 1:
                    if (val64 > UINT8_MAX)
                    {
                        val8 = UINT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val8 = (uint8_t) val64;
                    }
                    cont->setData(val8);
                    break;
                case 2:
                    if (val64 > UINT16_MAX)
                    {
                        val16 = UINT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val16 = (uint16_t) val64;
                    }
                    cont->setData(val16);
                    break;
                case 4:
                    if (val64 > UINT32_MAX)
                    {
                        val32 = UINT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val32 = (uint32_t) val64;
                    }
                    cont->setData(val32);
                    break;
                default:
                    cont->setData(val64);
                    break;
            }
            break;

        case DATA_TYPE_MEDINT:
        {
            // 2^23-1 is signed int max which is five Fs
            if (fromValue > (uint32_t)0xFFFFF)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            if (fromValue > INT32_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MAX;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            valF = (float) fromValue;
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
            val32 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val32);
            break;

        case DATA_TYPE_BIGINT:
        {
            if (fromValue > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MAX;
            }
            else
            {
                val64 = fromValue;
            }
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            valD = (double) fromValue;
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        case DATA_TYPE_DATETIME:
            val64 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val64);
            break;

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            valS = std::to_string(fromValue);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT8_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = UINT8_MAX - 2;
            }
            else
            {
                val8 = fromValue;
            }
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT16_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = UINT16_MAX;
            }
            else
            {
                val16 = fromValue;
            }
            cont->setData(val16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            // 2^24-1 is unsigned int max which is six Fs
            if (fromValue > (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF - 2;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT32_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = UINT32_MAX - 2;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_UBIGINT:
            // ColumnStore max 2 reserved
            if (fromValue > UINT64_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = UINT64_MAX - 2;
            }
            else
            {
                val64 = fromValue;
            }
            cont->setData(val64);
            break;

    }

    return status;
}

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, int64_t fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    uint8_t uval8;
    int16_t val16;
    uint16_t uval16;
    int32_t val32;
    int64_t val64;
    float valF;
    double valD;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            if (fromValue > 1)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = 1;
            }
            else
            {
                val8 = fromValue;
            }
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            if (fromValue > INT8_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MAX;
            }
            else if (fromValue < INT8_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MIN + 2;
            }
            else
            {
                val8 = fromValue;
            }
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            if (fromValue > INT16_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MAX;
            }
            else if (fromValue < INT16_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MIN + 2;
            }
            else
            {
                val16 = fromValue;
            }
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        case DATA_TYPE_DECIMAL:
            val64 = (int64_t)(fromValue * pow((double)10, toMeta->getScale()));
            switch (toMeta->getWidth())
            {
                case 1:
                    if (val64 > INT8_MAX)
                    {
                        val8 = INT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT8_MIN)
                    {
                        val8 = INT8_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val8 = (int8_t) val64;
                    }
                    cont->setData(val8);
                    break;
                case 2:
                    if (val64 > INT16_MAX)
                    {
                        val16 = INT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT16_MIN)
                    {
                        val16 = INT16_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val16 = (int16_t) val64;
                    }
                    cont->setData(val16);
                    break;
                case 4:
                    if (val64 > INT32_MAX)
                    {
                        val32 = INT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT32_MIN)
                    {
                        val32 = INT32_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val32 = (int32_t) val64;
                    }
                    cont->setData(val32);
                    break;
                default:
                    cont->setData(val64);
                    break;
            }
            break;

        case DATA_TYPE_MEDINT:
        {
            // 2^23-1 is signed int max which is five Fs
            if (fromValue > (int32_t)0xFFFFF)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF;
            }
            else if (fromValue < 0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFFF - 2;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            if (fromValue > INT32_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MAX;
            }
            else if (fromValue < INT32_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MIN + 2;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            valF = (float) fromValue;
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
            val32 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val32);
            break;

        case DATA_TYPE_BIGINT:
        {
            if (fromValue > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MAX;
            }
            else if (fromValue < INT64_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MIN + 2;
            }
            else
            {
                val64 = fromValue;
            }
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            valD = (double) fromValue;
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        case DATA_TYPE_DATETIME:
            val64 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val64);
            break;

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            valS = std::to_string(fromValue);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT8_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = UINT8_MAX - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = 0;
            }
            {
                uval8 = fromValue;
            }
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT16_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = UINT16_MAX;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = 0;
            }
            else
            {
                uval16 = fromValue;
            }
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            // 2^24-1 is unsigned int max which is six Fs
            if (fromValue > (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            // ColumnStore max 2 reserved
            if (fromValue > UINT32_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = UINT32_MAX - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0;
            }
            else
            {
                val32 = fromValue;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_UBIGINT:
            // ColumnStore max 2 reserved
            if (fromValue > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MAX;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = 0;
            }
            else
            {
                val64 = fromValue;
            }
            cont->setData(val64);
            break;

    }

    return status;

}

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, double fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    int16_t val16;
    int32_t val32;
    int64_t val64;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    float valF;
    double valD;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            uval8 = (uint8_t) fromValue;
            if (uval8 > 1)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = 1;
            }
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            val64 = (int64_t) fromValue;
            if (val64 > INT8_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MAX;
            }
            else if (val64 < INT8_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MIN + 2;
            }
            else
            {
                val8 = val64;
            }
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            val64 = (int64_t) fromValue;
            if (val64 > INT16_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MAX;
            }
            else if (val64 < INT16_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MIN + 2;
            }
            else
            {
                val16 = val64;
            }
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
            uval64 = (uint64_t)(fromValue * pow((double)10, toMeta->getScale()));
            switch (toMeta->getWidth())
            {
                case 1:
                    if (uval64 > UINT8_MAX)
                    {
                        uval8 = UINT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval8 = (uint8_t) uval64;
                    }
                    cont->setData(uval8);
                    break;
                case 2:
                    if (uval64 > UINT16_MAX)
                    {
                        uval16 = UINT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval16 = (uint16_t) uval64;
                    }
                    cont->setData(uval16);
                    break;
                case 4:
                    if (uval64 > UINT32_MAX)
                    {
                        uval32 = UINT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval32 = (uint32_t) uval64;
                    }
                    cont->setData(uval32);
                    break;
                default:
                    cont->setData(uval64);
                    break;
            }
            break;

        case DATA_TYPE_DECIMAL:
            val64 = (int64_t)(fromValue * pow((double)10, toMeta->getScale()));
            switch (toMeta->getWidth())
            {
                case 1:
                    if (val64 > INT8_MAX)
                    {
                        val8 = INT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT8_MIN)
                    {
                        val8 = INT8_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val8 = (int8_t) val64;
                    }
                    cont->setData(val8);
                    break;
                case 2:
                    if (val64 > INT16_MAX)
                    {
                        val16 = INT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT16_MIN)
                    {
                        val16 = INT16_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val16 = (int16_t) val64;
                    }
                    cont->setData(val16);
                    break;
                case 4:
                    if (val64 > INT32_MAX)
                    {
                        val32 = INT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT32_MIN)
                    {
                        val32 = INT32_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val32 = (int32_t) val64;
                    }
                    cont->setData(val32);
                    break;
                default:
                    cont->setData(val64);
                    break;
            }
            break;

        case DATA_TYPE_MEDINT:
        {
            val64 = (int64_t) fromValue;
            // 2^23-1 is signed int max which is five Fs
            if (val64 > (int32_t)0xFFFFF)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF;
            }
            else if (val64 < 0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFFF - 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            val64 = (int64_t) fromValue;
            if (val64 > INT32_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MAX;
            }
            else if (val64 < INT32_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MIN + 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        {
            valF = (float) fromValue;
            if (valF < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                valF = 0;
            }
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_FLOAT:
        {
            valF = (float) fromValue;
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
            val32 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val32);
            break;

        case DATA_TYPE_BIGINT:
        {
            val64 = (int64_t) fromValue;
            if (fromValue > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MAX;
            }
            else if (val64 < INT64_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MIN + 2;
            }
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        {
            valD = (double) fromValue;
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_UDOUBLE:
        {
            valD = (double) fromValue;
            if (valD < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                valD = 0;
            }
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        case DATA_TYPE_DATETIME:
            val64 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val64);
            break;

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            valS = std::to_string(fromValue);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            uval8 = (uint8_t) fromValue;
            // ColumnStore max 2 reserved
            if (fromValue > UINT8_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = UINT8_MAX - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = 0;
            }
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            uval16 = (uint16_t) fromValue;
            // ColumnStore max 2 reserved
            if (fromValue > UINT16_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = UINT16_MAX;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = 0;
            }
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            uval32 = (uint32_t) fromValue;
            // 2^24-1 is unsigned int max which is six Fs
            if (fromValue > (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0xFFFFF - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0;
            }
            cont->setData(uval32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            uval32 = (uint32_t) fromValue;
            // ColumnStore max 2 reserved
            if (fromValue > UINT32_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = UINT32_MAX - 2;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0;
            }
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_UBIGINT:
            uval64 = (uint64_t) fromValue;
            // ColumnStore max 2 reserved
            if (fromValue > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                uval64 = INT64_MAX;
            }
            else if (fromValue < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval64 = 0;
            }
            else
            {
                uval64 = fromValue;
            }
            cont->setData(uval64);
            break;

    }

    return status;
}

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, const std::string& fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    int16_t val16;
    int32_t val32;
    int64_t val64;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    float valF;
    double valD;
    ColumnStoreDateTimeImpl dTime;
    ColumnStoreTimeImpl tTime;
    std::string valStr;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val8 = 0;
                cont->setData(val8);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val8 = 0;
                cont->setData(val8);
                break;
            }

            if (val64 > 1)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = 1;
            }
            else
            {
                val8 = val64;
            }
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val8 = 0;
                cont->setData(val8);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val8 = 0;
                cont->setData(val8);
                break;
            }

            if (val64 > INT8_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MAX;
            }
            else if (val64 < INT8_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MIN + 2;
            }
            else
            {
                val8 = val64;
            }
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val16 = 0;
                cont->setData(val16);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val16 = 0;
                cont->setData(val16);
                break;
            }

            if (val64 > INT16_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MAX;
            }
            else if (val64 < INT16_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MIN + 2;
            }
            else
            {
                val16 = val64;
            }
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        case DATA_TYPE_DECIMAL:
        {
            try
            {
                valD = stod(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            val64 = (uint64_t)(valD * pow((double)10, toMeta->getScale()));
            switch (toMeta->getWidth())
            {
                case 1:
                    if (val64 > INT8_MAX)
                    {
                        val8 = INT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT8_MIN)
                    {
                        val8 = INT8_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val8 = (int8_t) val64;
                    }
                    cont->setData(val8);
                    break;
                case 2:
                    if (val64 > INT16_MAX)
                    {
                        val16 = INT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT16_MIN)
                    {
                        val16 = INT16_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val16 = (int16_t) val64;
                    }
                    cont->setData(val16);
                    break;
                case 4:
                    if (val64 > INT32_MAX)
                    {
                        val32 = INT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT32_MIN)
                    {
                        val32 = INT32_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val32 = (int32_t) val64;
                    }
                    cont->setData(val32);
                    break;
                default:
                    cont->setData(val64);
                    break;
            }

            break;
        }

        case DATA_TYPE_MEDINT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }

            // 2^23-1 is signed int max which is five Fs
            if (val64 > (uint32_t)0xFFFFF)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF;
            }
            else if (val64 < (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFFF - 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }

            if (val64 > INT32_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MAX;
            }
            else if (val64 < INT32_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MIN + 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            try
            {
                valF = stof(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val32 = 0;
                cont->setData(val32);
                break;
            }
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
            status = dTime.setFromString(fromValue);
            if (status == CONVERT_STATUS_NONE)
            {
                uval32 = dTime.getDateInt();
            }
            else
            {
                uval32 = 0;
            }
            cont->setData(uval32);
            break;

        case DATA_TYPE_BIGINT:
        {
            try
            {
                val64 = stoll(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            if (val64 > INT64_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MAX;
            }
            else if (val64 < INT64_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MIN + 2;
            }
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            try
            {
                valD = stod(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                val64 = 0;
                cont->setData(val64);
                break;
            }
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
            status = tTime.setFromString(fromValue);
            if (status == CONVERT_STATUS_NONE)
            {
                uval64 = tTime.getTimeInt();
            }
            else
            {
                uval64 = 0;
            }
            cont->setData(uval64);
            break;

        case DATA_TYPE_DATETIME:
            status = dTime.setFromString(fromValue);
            if (status == CONVERT_STATUS_NONE)
            {
                uval64 = dTime.getDateTimeInt();
            }
            else
            {
                uval64 = 0;
            }
            cont->setData(uval64);
            break;

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            if (fromValue.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valStr = fromValue.substr(0, toMeta->getWidth());
                cont->setData(valStr);
            }
            else
            {
                cont->setData(fromValue);
            }
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            try
            {
                uval64 = stoull(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                uval8 = 0;
                cont->setData(uval8);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                uval8 = 0;
                cont->setData(uval8);
                break;
            }

            // ColumnStore max 2 reserved
            if (uval64 > UINT8_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = UINT8_MAX - 2;
            }
            else
            {
                uval8 = uval64;
            }
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            try
            {
                uval64 = stoull(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                uval16 = 0;
                cont->setData(uval16);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                uval16 = 0;
                cont->setData(uval16);
                break;
            }

            // ColumnStore max 2 reserved
            if (uval64 > UINT16_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = UINT16_MAX;
            }
            else
            {
                uval16 = uval64;
            }
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            try
            {
                uval64 = stoull(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                uval32 = 0;
                cont->setData(uval32);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                uval32 = 0;
                cont->setData(uval32);
                break;
            }

            // 2^24-1 is unsigned int max which is six Fs
            if (uval64 > (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0xFFFFF - 2;
            }
            else
            {
                uval32 = uval64;
            }
            cont->setData(uval32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            try
            {
                uval64 = stoull(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                uval32 = 0;
                cont->setData(uval32);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                uval32 = 0;
                cont->setData(uval32);
                break;
            }

            // ColumnStore max 2 reserved
            if (uval64 > UINT32_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = UINT32_MAX - 2;
            }
            else
            {
                uval32 = uval64;
            }
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_UBIGINT:
            try
            {
                uval64 = stoull(fromValue);
            }
            catch (std::invalid_argument)
            {
                status = CONVERT_STATUS_INVALID;
                uval64 = 0;
                cont->setData(uval64);
                break;
            }
            catch (std::out_of_range)
            {
                status = CONVERT_STATUS_INVALID;
                uval64 = 0;
                cont->setData(uval64);
                break;
            }

            // ColumnStore max 2 reserved
            if (uval64 > UINT64_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval64 = UINT64_MAX - 2;
            }
            cont->setData(uval64);
            break;

    }

    return status;
}

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, ColumnStoreDateTime& fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    int16_t val16;
    int32_t val32;
    int64_t val64;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            status = CONVERT_STATUS_INVALID;
            val8 = 0;
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            status = CONVERT_STATUS_INVALID;
            val8 = 0;
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            status = CONVERT_STATUS_INVALID;
            val16 = 0;
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        case DATA_TYPE_DECIMAL:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_MEDINT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
        {
            uval32 = fromValue.mImpl->getDateInt();
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_BIGINT:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        {
            uval64 = fromValue.mImpl->getTimeInt();
            cont->setData(uval64);
            break;
        }

        case DATA_TYPE_DATETIME:
        {
            uval64 = fromValue.mImpl->getDateTimeInt();
            cont->setData(uval64);
            break;
        }

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            fromValue.mImpl->getDateTimeStr(valS);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval8 = 0;
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval16 = 0;
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval32 = 0;
            cont->setData(uval32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval32 = 0;
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_UBIGINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval64 = 0;
            cont->setData(uval64);
            break;
        }
    }

    return status;
}

columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, ColumnStoreTime& fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    int16_t val16;
    int32_t val32;
    int64_t val64;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            status = CONVERT_STATUS_INVALID;
            val8 = 0;
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            status = CONVERT_STATUS_INVALID;
            val8 = 0;
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            status = CONVERT_STATUS_INVALID;
            val16 = 0;
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        case DATA_TYPE_DECIMAL:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_MEDINT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
        {
            status = CONVERT_STATUS_INVALID;
            val32 = 0;
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_BIGINT:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            status = CONVERT_STATUS_INVALID;
            val64 = 0;
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        {
            uval64 = fromValue.mImpl->getTimeInt();
            cont->setData(uval64);
            break;
        }

        case DATA_TYPE_DATETIME:
        {
            uval64 = fromValue.mImpl->getDateTimeInt();
            cont->setData(uval64);
            break;
        }

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            fromValue.mImpl->getTimeStr(valS);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval8 = 0;
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval16 = 0;
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval32 = 0;
            cont->setData(uval32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval32 = 0;
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_UBIGINT:
        {
            status = CONVERT_STATUS_INVALID;
            uval64 = 0;
            cont->setData(uval64);
            break;
        }
    }

    return status;
}


columnstore_data_convert_status_t ColumnStoreDataConvert::convert(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont, ColumnStoreDecimal& fromValue)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    int8_t val8;
    int16_t val16;
    int32_t val32;
    int64_t val64;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    float valF;
    double valD;
    std::string valS;
    switch(toMeta->getType())
    {
        case DATA_TYPE_BIT:
        {
            val64 = fromValue.mImpl->getInt();
            if (val64 > 1)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = 1;
            }
            else if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = 0;
            }
            else
            {
                val8 = val64;
            }
            cont->setData(val8);
            break;
        }
        case DATA_TYPE_TINYINT:
        {
            val64 = fromValue.mImpl->getInt();
            if (val64 > INT8_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MAX;
            }
            else if (val64 < INT8_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val8 = INT8_MIN + 2;
            }
            else
            {
                val8 = val64;
            }
            cont->setData(val8);
            break;
        }

        case DATA_TYPE_SMALLINT:
        {
            val64 = fromValue.mImpl->getInt();
            if (val64 > INT16_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MAX;
            }
            else if (val64 < INT16_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val16 = INT16_MIN + 2;
            }
            else
            {
                val16 = val64;
            }
            cont->setData(val16);
            break;
        }

        case DATA_TYPE_UDECIMAL:
        {
            uval64 = fromValue.mImpl->getDecimalInt(toMeta->getScale());
            switch (toMeta->getWidth())
            {
                case 1:
                    if (uval64 > UINT8_MAX)
                    {
                        uval8 = UINT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval8 = (uint8_t) uval64;
                    }
                    cont->setData(uval8);
                    break;
                case 2:
                    if (uval64 > UINT16_MAX)
                    {
                        uval16 = UINT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval16 = (uint16_t) uval64;
                    }
                    cont->setData(uval16);
                    break;
                case 4:
                    if (uval64 > UINT32_MAX)
                    {
                        uval32 = UINT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        uval32 = (uint32_t) uval64;
                    }
                    cont->setData(uval32);
                    break;
                default:
                    cont->setData(uval64);
                    break;
            }
            break;
        }
        case DATA_TYPE_DECIMAL:
        {
            val64 = fromValue.mImpl->getDecimalInt(toMeta->getScale());
            switch (toMeta->getWidth())
            {
                case 1:
                    if (val64 > INT8_MAX)
                    {
                        val8 = INT8_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT8_MIN)
                    {
                        val8 = INT8_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val8 = (int8_t) val64;
                    }
                    cont->setData(val8);
                    break;
                case 2:
                    if (val64 > INT16_MAX)
                    {
                        val16 = INT16_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT16_MIN)
                    {
                        val16 = INT16_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val16 = (int16_t) val64;
                    }
                    cont->setData(val16);
                    break;
                case 4:
                    if (val64 > INT32_MAX)
                    {
                        val32 = INT32_MAX;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else if (val64 < INT32_MIN)
                    {
                        val32 = INT32_MIN;
                        status = CONVERT_STATUS_SATURATED;
                    }
                    else
                    {
                        val32 = (int32_t) val64;
                    }
                    cont->setData(val32);
                    break;
                default:
                    cont->setData(val64);
                    break;
            }
            break;
        }


        case DATA_TYPE_MEDINT:
        {
            val64 = fromValue.mImpl->getInt();

            // 2^23-1 is signed int max which is five Fs
            if (val64 > (uint32_t)0xFFFFF)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFF;
            }
            else if (val64 < (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = 0xFFFFFF - 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_INT:
        {
            val64 = fromValue.mImpl->getInt();

            if (val64 > INT32_MAX)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MAX;
            }
            else if (val64 < INT32_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val32 = INT32_MIN + 2;
            }
            else
            {
                val32 = val64;
            }
            cont->setData(val32);
            break;
        }
        case DATA_TYPE_UFLOAT:
        case DATA_TYPE_FLOAT:
        {
            valF = (float)fromValue.mImpl->getDouble();
            memcpy(&val32, &valF, 4);
            cont->setData(val32);
            break;
        }

        case DATA_TYPE_DATE:
            val32 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val32);
            break;

        case DATA_TYPE_BIGINT:
        {
            val64 = fromValue.mImpl->getInt();

            if (val64 < INT64_MIN + 2)
            {
                status = CONVERT_STATUS_SATURATED;
                val64 = INT64_MIN + 2;
            }
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_UDOUBLE:
        {
            valD = fromValue.mImpl->getDouble();
            memcpy(&val64, &valD, 8);
            cont->setData(val64);
            break;
        }

        case DATA_TYPE_TIME:
        case DATA_TYPE_DATETIME:
            val64 = 0;
            status = CONVERT_STATUS_INVALID;
            cont->setData(val64);
            break;

        case DATA_TYPE_VARCHAR:
        case DATA_TYPE_CHAR:
        case DATA_TYPE_TEXT:
        case DATA_TYPE_VARBINARY:
        case DATA_TYPE_CLOB:
        case DATA_TYPE_BLOB:
        {
            fromValue.mImpl->getDecimalStr(valS);
            if (valS.length() > toMeta->getWidth())
            {
                status = CONVERT_STATUS_TRUNCATED;
                valS.resize(toMeta->getWidth());
            }
            cont->setData(valS);
            break;
        }

        case DATA_TYPE_UTINYINT:
        {
            val64 = fromValue.mImpl->getInt();
            // ColumnStore max 2 reserved
            if (val64 > UINT8_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = UINT8_MAX - 2;
            }
            else if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval8 = 0;
            }
            else
            {
                uval8 = val64;
            }
            cont->setData(uval8);
            break;
        }
        case DATA_TYPE_USMALLINT:
        {
            val64 = fromValue.mImpl->getInt();
            // ColumnStore max 2 reserved
            if (val64 > UINT16_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = UINT16_MAX;
            }
            else if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval16 = 0;
            }
            else
            {
                uval16 = val64;
            }
            cont->setData(uval16);
            break;
        }
        case DATA_TYPE_UMEDINT:
        {
            val64 = fromValue.mImpl->getInt();
            // 2^24-1 is unsigned int max which is six Fs
            if (val64 > (uint32_t)0xFFFFFF - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0xFFFFF - 2;
            }
            else if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0;
            }
            else
            {
                uval32 = val64;
            }
            cont->setData(uval32);
            break;
        }
        case DATA_TYPE_UINT:
        {
            val64 = fromValue.mImpl->getInt();
            // ColumnStore max 2 reserved
            if (val64 > UINT32_MAX - 2)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = UINT32_MAX;
            }
            else if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval32 = 0;
            }
            else
            {
                uval32 = val64;
            }
            cont->setData(uval32);
            break;
        }

        case DATA_TYPE_UBIGINT:
            val64 = fromValue.mImpl->getInt();
            if (val64 < 0)
            {
                status = CONVERT_STATUS_SATURATED;
                uval64 = 0;
            }
            else
            {
                uval64 = val64;
            }
            cont->setData(uval64);
            break;
    }

    return status;
}

columnstore_data_convert_status_t ColumnStoreDataConvert::getNull(ColumnStoreSystemCatalogColumn* toMeta, ColumnStoreDataContainer* cont)
{
    columnstore_data_convert_status_t status = CONVERT_STATUS_NONE;
    uint8_t uval8;
    uint16_t uval16;
    uint32_t uval32;
    uint64_t uval64;
    std::string valStr;

    // Don't set null for not-null, but autoinc is OK
    if (!toMeta->isNullable() && !toMeta->isAutoincrement())
    {
        status = CONVERT_STATUS_INVALID;
        switch(toMeta->getType())
        {
            case DATA_TYPE_BIT:
            {
                uval8 = 0;
                cont->setData(uval8);
                break;
            }
            case DATA_TYPE_TINYINT:
            {
                uval8 = 0;
                cont->setData(uval8);
                break;
            }

            case DATA_TYPE_SMALLINT:
            {
                uval16 = 0;
                cont->setData(uval16);
                break;
            }

            case DATA_TYPE_BIGINT:
            case DATA_TYPE_DECIMAL:
            {
                uval64 = 0;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_UBIGINT:
            case DATA_TYPE_UDECIMAL:
            {
                uval64 = 0;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_INT:
            case DATA_TYPE_MEDINT:
            {
                uval32 = 0;
                cont->setData(uval32);
                break;
            }
            case DATA_TYPE_UFLOAT:
            case DATA_TYPE_FLOAT:
            {
                uval32 = 0;
                cont->setData(uval32);
                break;
            }

            case DATA_TYPE_UINT:
            case DATA_TYPE_UMEDINT:
            case DATA_TYPE_DATE:
            {
                uval32 = 0;
                cont->setData(uval32);
                break;
            }

            case DATA_TYPE_DOUBLE:
            case DATA_TYPE_UDOUBLE:
            {
                uval64 = 0;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_TIME:
            case DATA_TYPE_DATETIME:
                uval64 = 0;
                cont->setData(uval64);
                break;

            case DATA_TYPE_VARCHAR:
            case DATA_TYPE_CHAR:
            case DATA_TYPE_TEXT:
            case DATA_TYPE_VARBINARY:
            case DATA_TYPE_CLOB:
            case DATA_TYPE_BLOB:
            {
                valStr = "";
                cont->setData(valStr);
                break;
            }

            case DATA_TYPE_UTINYINT:
            {
                uval8 = 0;
                cont->setData(uval8);
                break;
            }
            case DATA_TYPE_USMALLINT:
            {
                uval16 = 0;
                cont->setData(uval16);
                break;
            }
        }

    }
    else
    {
        // This flag doesn't work yet
        cont->isNull = true;

        switch(toMeta->getType())
        {
            case DATA_TYPE_BIT:
            {
                uval8 = 0xFE;
                cont->setData(uval8);
                break;
            }
            case DATA_TYPE_TINYINT:
            {
                uval8 = 0x80;
                cont->setData(uval8);
                break;
            }

            case DATA_TYPE_SMALLINT:
            {
                uval16 = 0x8000;
                cont->setData(uval16);
                break;
            }

            case DATA_TYPE_BIGINT:
            case DATA_TYPE_DECIMAL:
            {
                uval64 = 0x8000000000000000ULL;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_UBIGINT:
            case DATA_TYPE_UDECIMAL:
            {
                uval64 = 0xFFFFFFFFFFFFFFFEULL;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_INT:
            case DATA_TYPE_MEDINT:
            {
                uval32 = 0x80000000;
                cont->setData(uval32);
                break;
            }
            case DATA_TYPE_UFLOAT:
            case DATA_TYPE_FLOAT:
            {
                uval32 = 0xFFAAAAAA;
                cont->setData(uval32);
                break;
            }

            case DATA_TYPE_UINT:
            case DATA_TYPE_UMEDINT:
            case DATA_TYPE_DATE:
            {
                uval32 = 0xFFFFFFFE;
                cont->setData(uval32);
                break;
            }

            case DATA_TYPE_DOUBLE:
            case DATA_TYPE_UDOUBLE:
            {
                uval64 = 0xFFFAAAAAAAAAAAAAULL;
                cont->setData(uval64);
                break;
            }

            case DATA_TYPE_TIME:
            case DATA_TYPE_DATETIME:
                uval64 = 0xFFFFFFFFFFFFFFFEULL;
                cont->setData(uval64);
                break;

            case DATA_TYPE_VARCHAR:
            case DATA_TYPE_CHAR:
            case DATA_TYPE_TEXT:
            case DATA_TYPE_VARBINARY:
            case DATA_TYPE_CLOB:
            case DATA_TYPE_BLOB:
            {
                // Note: this will need fixing when we support char NULL properly
                valStr = "";
                cont->setData(valStr);
                break;
            }

            case DATA_TYPE_UTINYINT:
            {
                uval8 = 0xFE;
                cont->setData(uval8);
                break;
            }
            case DATA_TYPE_USMALLINT:
            {
                uval16 = 0xFFFE;
                cont->setData(uval16);
                break;
            }
        }
    }
    return status;
}

}
