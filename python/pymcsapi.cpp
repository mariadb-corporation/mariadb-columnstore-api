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
 
#include <pybind11/pybind11.h> 
#include <mcsapi.h>

/*
 * Python interface to MariaDB ColumnStore API. Uses pybind11 library to provide python bindings to mcsapi.h
 */
 
namespace py = pybind11;
using namespace mcsapi;

PYBIND11_MODULE(pymcsapi, m) {

    py::class_<ColumnStoreDriver>(m, "ColumnStoreDriver")
    .def(py::init())
    .def(py::init<const std::string &>())
    .def("getVersion", &ColumnStoreDriver::getVersion)
    .def("createBulkInsert", &ColumnStoreDriver::createBulkInsert)
    .def("getSystemCatalog", &ColumnStoreDriver::getSystemCatalog); 

    
    py::class_<ColumnStoreBulkInsert>(m, "ColumnStoreBulkInsert")
    .def("getColumnCount", &ColumnStoreBulkInsert::getColumnCount)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, const std::string&, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set string column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, uint64_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set uint64_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, int64_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set int64_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, uint32_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set uint32_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, int32_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set int32_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, uint16_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set uint16_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, int16_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set int16_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, uint8_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set uint8_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, int8_t, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set int8_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)    
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, double, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set double column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)   
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, ColumnStoreDateTime&, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set ColumnStoreDateTime column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)   
    .def("setColumn", (ColumnStoreBulkInsert* (ColumnStoreBulkInsert::*) (uint16_t, ColumnStoreDecimal&, columnstore_data_convert_status_t*)) &ColumnStoreBulkInsert::setColumn, "Set int8_t column value", py::arg("columnNumber"), py::arg("value"), py::arg("status") = nullptr)    
    .def("setNull", &ColumnStoreBulkInsert::setNull)
    .def("writeRow", &ColumnStoreBulkInsert::writeRow)
    .def("rollback", &ColumnStoreBulkInsert::rollback)
    .def("commit", &ColumnStoreBulkInsert::commit)
    .def("getSummary", &ColumnStoreBulkInsert::getSummary)
    .def("setTruncateIsError", &ColumnStoreBulkInsert::setTruncateIsError)
    .def("setBatchSize", &ColumnStoreBulkInsert::setBatchSize); 

    
    py::class_<ColumnStoreSystemCatalog>(m, "ColumnStoreSystemCatalog")
    .def("getTable", &ColumnStoreSystemCatalog::getTable);  

    
    py::class_<ColumnStoreSystemCatalogTable>(m, "ColumnStoreSystemCatalogTable")
    .def("getSchemaName", &ColumnStoreSystemCatalogTable::getSchemaName)
    .def("getTableName", &ColumnStoreSystemCatalogTable::getTableName)    
    .def("getOID", &ColumnStoreSystemCatalogTable::getOID)
    .def("getColumnCount", &ColumnStoreSystemCatalogTable::getColumnCount)
    .def("getColumn", (ColumnStoreSystemCatalogColumn& (ColumnStoreSystemCatalogTable::*) (const std::string& )) &ColumnStoreSystemCatalogTable::getColumn, py::arg("columnName"))
    .def("getColumn", (ColumnStoreSystemCatalogColumn& (ColumnStoreSystemCatalogTable::*) (uint16_t )) &ColumnStoreSystemCatalogTable::getColumn, py::arg("columnNumber"))
    ;  

    py::class_<ColumnStoreSystemCatalogColumn>(m, "ColumnStoreSystemCatalogColumn")
    .def("getOID", &ColumnStoreSystemCatalogColumn::getOID)    
    .def("getColumnName", &ColumnStoreSystemCatalogColumn::getColumnName)  
    .def("getDictionaryOID", &ColumnStoreSystemCatalogColumn::getDictionaryOID)    
    .def("getType", &ColumnStoreSystemCatalogColumn::getType)  
    .def("getWidth", &ColumnStoreSystemCatalogColumn::getWidth)    
    .def("getPosition", &ColumnStoreSystemCatalogColumn::getPosition)  
    .def("getDefaultValue", &ColumnStoreSystemCatalogColumn::getDefaultValue)    
    .def("isAutoincrement", &ColumnStoreSystemCatalogColumn::isAutoincrement)  
    .def("getPrecision", &ColumnStoreSystemCatalogColumn::getPrecision)    
    .def("getPosition", &ColumnStoreSystemCatalogColumn::getPosition)  
    .def("getScale", &ColumnStoreSystemCatalogColumn::getScale)    
    .def("isNullable", &ColumnStoreSystemCatalogColumn::isNullable)  
    .def("compressionType", &ColumnStoreSystemCatalogColumn::compressionType);
    
    
    py::enum_<columnstore_data_types_t>(m, "columnstore_data_types_t")
    .value("DATA_TYPE_BIT", columnstore_data_types_t::DATA_TYPE_BIT)
    .value("DATA_TYPE_TINYINT", columnstore_data_types_t::DATA_TYPE_TINYINT)
    .value("DATA_TYPE_CHAR", columnstore_data_types_t::DATA_TYPE_CHAR)
    .value("DATA_TYPE_SMALLINT", columnstore_data_types_t::DATA_TYPE_SMALLINT)
    .value("DATA_TYPE_DECIMAL", columnstore_data_types_t::DATA_TYPE_DECIMAL)
    .value("DATA_TYPE_MEDINT", columnstore_data_types_t::DATA_TYPE_MEDINT)
    .value("DATA_TYPE_INT", columnstore_data_types_t::DATA_TYPE_INT)
    .value("DATA_TYPE_FLOAT", columnstore_data_types_t::DATA_TYPE_FLOAT)
    .value("DATA_TYPE_DATE", columnstore_data_types_t::DATA_TYPE_DATE)
    .value("DATA_TYPE_BIGINT", columnstore_data_types_t::DATA_TYPE_BIGINT)
    .value("DATA_TYPE_DOUBLE", columnstore_data_types_t::DATA_TYPE_DOUBLE)
    .value("DATA_TYPE_DATETIME", columnstore_data_types_t::DATA_TYPE_DATETIME)
    .value("DATA_TYPE_VARCHAR", columnstore_data_types_t::DATA_TYPE_VARCHAR)
    .value("DATA_TYPE_VARBINARY", columnstore_data_types_t::DATA_TYPE_VARBINARY)
    .value("DATA_TYPE_CLOB", columnstore_data_types_t::DATA_TYPE_CLOB)
    .value("DATA_TYPE_BLOB", columnstore_data_types_t::DATA_TYPE_BLOB)
    .value("DATA_TYPE_UTINYINT", columnstore_data_types_t::DATA_TYPE_UTINYINT)
    .value("DATA_TYPE_USMALLINT", columnstore_data_types_t::DATA_TYPE_USMALLINT)
    .value("DATA_TYPE_UDECIMAL", columnstore_data_types_t::DATA_TYPE_UDECIMAL)
    .value("DATA_TYPE_UMEDINT", columnstore_data_types_t::DATA_TYPE_UMEDINT)
    .value("DATA_TYPE_UINT", columnstore_data_types_t::DATA_TYPE_UINT)
    .value("DATA_TYPE_UFLOAT", columnstore_data_types_t::DATA_TYPE_UFLOAT)
    .value("DATA_TYPE_UBIGINT", columnstore_data_types_t::DATA_TYPE_UBIGINT)
    .value("DATA_TYPE_UDOUBLE", columnstore_data_types_t::DATA_TYPE_UDOUBLE)    
    .value("DATA_TYPE_TEXT", columnstore_data_types_t::DATA_TYPE_TEXT)   
    .export_values(); 
     
     
    py::enum_<columnstore_data_convert_status_t>(m, "columnstore_data_convert_status_t")
    .value("CONVERT_STATUS_NONE", columnstore_data_convert_status_t::CONVERT_STATUS_NONE)
    .value("CONVERT_STATUS_SATURATED", columnstore_data_convert_status_t::CONVERT_STATUS_SATURATED)
    .value("CONVERT_STATUS_INVALID", columnstore_data_convert_status_t::CONVERT_STATUS_INVALID)
    .value("CONVERT_STATUS_TRUNCATED", columnstore_data_convert_status_t::CONVERT_STATUS_TRUNCATED)
    .export_values();     
    
    
    py::class_<ColumnStoreSummary>(m, "ColumnStoreSummary")
    .def("getExecutionTime", &ColumnStoreSummary::getExecutionTime)
    .def("getRowsInsertedCount", &ColumnStoreSummary::getRowsInsertedCount)
    .def("getTruncationCount", &ColumnStoreSummary::getTruncationCount)
    .def("getSaturatedCount", &ColumnStoreSummary::getSaturatedCount)
    .def("getInvalidCount", &ColumnStoreSummary::getInvalidCount);   
    
    
    py::class_<ColumnStoreDateTime>(m, "ColumnStoreDateTime")
    .def(py::init())
    .def(py::init<uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint32_t >(), py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour") = 0, py::arg("minute") = 0, py::arg("second") = 0, py::arg("microsecond") = 0) 
    .def(py::init<const std::string &,const std::string &>());
     

    py::class_<ColumnStoreDecimal>(m, "ColumnStoreDecimal")
    .def(py::init())
    .def(py::init<int64_t>()) 
    .def(py::init<const std::string &>())
    .def(py::init<double>()) 
    .def(py::init<int64_t, uint8_t>());      
}



