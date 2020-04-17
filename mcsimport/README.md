# MariaDB ColumnStore mcsimport

This tool imports data from a csv file into a remote ColumnStore instance utilizing the Bulk Write SDK.

## Build instructions

The adapter depends on following libraries.

* [yaml-cpp]

### Linux

#### Install the yaml-cpp dependency
```shell
git clone https://github.com/jbeder/yaml-cpp
cd yaml-cpp
git checkout yaml-cpp-0.6.2
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
make -j2
sudo make install
```

#### Build and install mcsimport
```shell
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api
mkdir build && cd build
cmake .. -DREMOTE_CPIMPORT=ON 
make
sudo make install
```

### Windows
On Windows you need to install Visual Studio with the "Visual Studio 2015 (v140)" platform toolset or greater.

#### Install the yaml-cpp dependency
```shell
git clone https://github.com/jbeder/yaml-cpp
cd yaml-cpp
git checkout yaml-cpp-0.6.2
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TESTS=OFF -G "Visual Studio 14 2015 Win64" ..
cmake --build . --config RelWithDebInfo
```

Afterwards set the environment variable `YAML_CPP_INSTALL_DIR` to the cloned yaml-cpp repository top level directory. (e.g. `C:\yaml-cpp`) 

#### Build and package mcsimport
```shell
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api
mkdir build && cd build
cmake .. -DREMOTE_CPIMPORT=ON -G "Visual Studio 15 2017 Win64" 
cmake --build . --config RelWithDebInfo --target package
signtool.exe sign /tr http://timestamp.digicert.com /td sha256 /fd sha256 /a "MariaDB ColumnStore mcsimport-*-x64.msi"
```

## Usage
```shell
mcsimport database table input_file [-m mapping_file] [-c Columnstore.xml] [-d delimiter] [-n null_option] [-df date_format] [-default_non_mapped] [-E enclose_by_character] [-C escape_character] [-rc read_cache_size] [-header] [-ignore_malformed_csv] [-err_log]
```

### -m mapping_file
The mapping file is used to define the mapping between source csv columns and target columnstore columns, to define column specific input date formats, and to set default values for ignored target columns. It follows the Yaml 1.2 standard and can address the source csv columns implicit and explicit.  
Source csv columns can only be identified by their position in the csv file starting with 0, and target columnstore columns can be identified either by their position or name.

Following snippet is an example for an implicit mapping file.
```
- column:
  target: 0
- column:
  - ignore
- column:
  target: id
- column:
  target: occurred
  format: "%d %b %Y %H:%M:%S"
- target: 2
  value: default
- target: salary
  value: 20000
```
It defines that the first csv column (#0) is mapped to the first column in the columnstore table, that the second csv column (#1) is ignored and won't be injected into the target table, that the third csv column (#2) is mapped to the columnstore column with the name `id`, and that the fourth csv column (#3) is mapped to the columnstore column with the name `occurred` and uses a specific date format. (defined using the [strptime] format)
The mapping file further defines that for the third columnstore column (#2) its default value will be used, and that the columnstore target column with the name `salary` will be set to 20000 for all injections. 


Explicit mapping is also possible.
```
- column: 0
  target: id
- column: 4
  target: salary
- target: timestamp
  value: 2018-09-13 12:00:00
```
Using this variant the first (#0) csv source column is mapped to the target columnstore column with the name `id`, and the fifth source csv column (#4) is mapped to the target columnstore column with the name `salary`. It further defines that the target columnstore column `timestamp` uses a default value of `2018-09-13 12:00:00` for the injection.

### -c Columnstore.xml
By default mcsimport uses the standard configuration file `/etc/columnstore/Columnstore.xml` or if set the one defined through the environment variable `COLUMNSTORE_INSTALL_DIR` to connect to the remote Columnstore instance. Individual configurations can be defined through the command line parameter -c. Information on how to create individual Columnstore.xml files can be found in our [Knowledge Base]. 

### -d delimiter
The default delimiter of the CSV input file is a comma `,` and can be changed through the command line parameter -d. Only one character delimiters are currently supported.

### -df date_format
By default mcsimport uses `YYYY-MM-DD HH:MM:SS` as input date format. An individual global date format can be specified via the command line parameter -df using the [strptime] format. Column specific input date formats can be defined in the mapping file and overwrite the global date format.

### -n null_option
By default mcsimport treats input strings with the value "NULL" as data. If the null_option is set to 1 strings with the value "NULL" are treated as NULL values.

### -default_non_mapped
mcsimport needs to inject values for all columnstore columns. In order to use the columnstore column's default values for all non mapped target columns the global parameter `default_non_mapped` can be used. Target column specific default values in the mapping file overwrite the global default values of this parameter.

### -E enclose_by_character
By default mcsimport uses the double-quote character `"` as enclosing character. It can be changed through the command line parameter -E. The enclosing character's length is limited to 1.

### -C escape_character
By default mcsimport uses the double-quote character `"` as escaping character. It can be changed through the command line parameter -C. The escaping character's length is limited to 1.

### -rc read_cache_size
By default mcsimport uses a read cache size of 20,971,520 (20 MiB) to cache chunks of the input file in RAM. It can be changed through the command line paramter -rc. A minimum cache size of 1,048,576 (1 MiB) is required.

### -header
Choose this flag to ignore the first line of the input CSV file as header. (It won't be injected)

### -ignore_malformed_csv
By default mcsimport rolls back the entire bulk import if a malformed csv entry is found. With this option mcsimport ignores detected malformed csv entries and continiues with the injection.

### -err_log
With this option an optional error log file is written which states truncated, saturated, and invalid values during the injection. If the command line parameter -ignore_malformed_csv is chosen, it also states which lines were ignored.

[yaml-cpp]: https://github.com/jbeder/yaml-cpp
[strptime]: http://pubs.opengroup.org/onlinepubs/9699919799/functions/strptime.html
[Knowledge Base]: https://mariadb.com/kb/en/library/columnstore-bulk-write-sdk/#environment-configuration
