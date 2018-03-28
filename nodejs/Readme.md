# MariaDB ColumnStore API NodeJS Wrapper
This provides a NodeJS wrapper around the C++ MariaDB ColumnStore API. It takes advantage of [SWIG](http://www.swig.org) to provide an almost identical interface to the C++ SDK but in NodeJS.

**NOTE:** The NodeJS Wrapper is highly EXPERIMENTAL and not recommended to be used in production.

**NOTE:** Currently no exception handling is implemented.

**NOTE:** Currently only NodeJS 6 is supported.

# Building
SWIG >= 3.0.9, NodeJS 6, and node-gyp must be installed on the server and in the path.

After the bulk write API is built you can execute the script make.sh manually to build the NodeJS binding.

```sh
./make.sh
```

# Testing
To test nodemcsapi you have to install mariasql and mocha via:

```sh
npm install mariasql
npm install mocha
```

Afterwards you can run the tests via:

```sh
npm test
```

# API changes
nodemcsapi needed to rename following overloaded C++ functions to work properly.

```
setColumn(uint16_t, uint64_t) ----> setColumn_uint64(column, value)
setColumn(uint16_t, int64_t)  ----> setColumn_int64(column, value)
setColumn(uint16_t, uint32_t) ----> setColumn_uint32(column, value)
setColumn(uint16_t, int32_t)  ----> setColumn_int32(column, value)
setColumn(uint16_t, uint16_t) ----> setColumn_uint16(column, value)
setColumn(uint16_t, int16_t)  ----> setColumn_int16(column, value)
setColumn(uint16_t, uint8_t)  ----> setColumn_uint8(column, value)
setColumn(uint16_t, int8_t)   ----> setColumn_int8(column, value)
setColumn(uint16_t, double)   ----> setColumn_double(column, value)
```

# Usage
Here is an example on how to use nodemcsapi from command line:

```sh
nodejs
> var nodemcsapi = require("./build/Release/nodemcsapi");
undefined
> var driver = new nodemcsapi.ColumnStoreDriver();
undefined
> var catalog = driver.getSystemCatalog();
undefined
> var table = catalog.getTable("test", "tmp1");
undefined
> table.getOID();
10082
> var bulk = driver.createBulkInsert("test", "tmp1", 0, 0);
undefined
> bulk.setColumn_int8(0, 1);
_exports_ColumnStoreBulkInsert {}
> bulk.setColumn(1, "abc");
_exports_ColumnStoreBulkInsert {}
> bulk.writeRow();
_exports_ColumnStoreBulkInsert {}
> bulk.commit();
undefined
```
