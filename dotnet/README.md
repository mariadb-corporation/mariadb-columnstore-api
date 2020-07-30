# MariaDB ColumnStore API .NET Wrapper - Alpha

.NET Core wrapper on the ColumnStore C++ API for Linux (x64) and Windows (x64).

## Getting Started

Riffing off of [MariaDB ColumnStore Bulk Write SDK](https://mariadb.com/kb/en/library/columnstore-bulk-write-sdk/)...

First a simple table is created with the mcsmysql client:

```sql
MariaDB [mcsapi]> create table t1(i int, c char(3)) engine=columnstore;
```

Next, create a new console project

```shell
mkdir ColumnStoreTestProgram
cd ColumnStoreTestProgram
dotnet new console
```

Alter Program.cs

```cs
using System;
using MariaDB.Data.ColumnStore;

namespace ColumnStoreTestProgram
{
    class Program
    {
        static void Main(string[] args)
        {
            // For windows, name the XML file location
            var ColumnstoreXmlFilePath = @"C:\opt\mariadb\etc\Columnstore.xml";
            var d = new ColumnStoreDriver(ColumnstoreXmlFilePath);
            var b = d.createBulkInsert("mcsapi", "t1", 0, 0);
            try
            {
                b.setColumn(0, 2);
                b.setColumn(1, "c#!");
                b.writeRow();
                b.commit();
            }
            catch (Exception e)
            {
                b.rollback();
                Console.WriteLine(e.StackTrace);
            }
        }
    }
}
```

And run

```shell
dotnet run
```

Now back in mcsmysql verify the data is written:

```misc
MariaDB [test]> select * from t1;
+------+------+
| i    | c    |
+------+------+
|    2 | c#!  |
+------+------+
```

### Windows

Note that when using this driver from windows, one must use the constructor that
takes a string pointing to the ```Columnstore.xml``` file, as shown above, otherwise
it will throw an exception since by default the constructor will look in UNIX-y type
locations.

```cs
using MariaDB.Data.ColumnStore;
...
var mcsConnection = new ColumnStoreDriver(@"C:\opt\mariadb\etc\Columnstore.xml");
```

### Further Examples

Also look at ```dotnet/test/ColumnStoreTest.cs``` for some hints on how to use this
package with some startup checks.

## Building the NuGet Package

Building the combination NuGet package requires a build environment for each
target platform. The simplest way to build the Linux + Win combination is by
using Windows 10 with gitbash coupled with the Windows Subsystem for Linux. (Of course
it is also possible to compile the wrapper on each platform and copy the needed
files over to a single system for the final NuGet pack.)

Requirements:

* Windows 10
* [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/)
* [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) (with a c++ development environment)
* [gitbash](https://git-scm.com/downloads)
* The other build requirements for the API

### Get the Source Code

For building the Linux shared object with WSL, the source code needs to be located
on a Windows partition because WSL can see windows files (under /mnt/DRIVE) but
not the other way around.

In Windows (gitbash):

```shell
mkdir ~/Code/external
cd ~/Code/external
git clone https://github.com/mariadb-corporation/mariadb-columnstore-api.git
cd ~/Code/external/mariadb-columnstore-api
```

### Build the Windows API DLL

The main README contains instructions on how to build the windows DLL.
Or try using [build-win-x64.sh](https://gist.github.com/TikiBill/3dc83d90e5337eb168da82c207f077e3)
from a windows gitbash prompt. Copy that file into mariadb-columnstore-api/extra and then run
it from gitbash.

### Copy the Windows DLLs

The resulting API DLL and LIB, along with the support DLLs (libxml2, libuv, libiconv), need to
be copied into dotnet/lib-win-x64. Assuming your build directory was build-win-x64:

```shell
cd ~/Code/external/mariadb-columnstore-api
mkdir dotnet/lib-win-x64
cp -v build-win-x64/src/RelWithDebInfo/mcsapi.dll dotnet/lib-win-x64
cp -v build-win-x64/src/RelWithDebInfo/mcsapi.lib dotnet/lib-win-x64
cp -v path/to/lib/libiconv.dll dotnet/lib-win-x64
cp -v path/to/lib/libuv.dll dotnet/lib-win-x64
cp -v path/to/lib/libxml2.dll dotnet/lib-win-x64
```

### Build the .NET Glue (Windows)

Run ```dotnet/build.sh``` to walk through building the windows glue dll and
c# files. This will put ```dotnet_mcsapi.dll``` in ```dotnet/lib-win-x64``` as
well as the .cs source files in ```dotnet/src```.

Note this script will also try to copy the DLLs outlined above if you have not
already done so.

### Build and Copy the Linux API Shared Object (.so)

Next, from WSL (or a linux box), follow the main README instructions to build
the shared mcsapi.so library. The commands would be (assuming the C: drive
contains your user directory, and your user name is USER):

```shell
cd  /mnt/c/Users/USER/Code/external/mariadb-columnstore-api
mkdir dotnet/lib-linux-x64
mkdir build-linux-x64
cd build-linux-x64
cmake -DJAVA=OFF -DPYTHON=OFF ..
make
sudo make install
cp src/libmcsapi.* ../dotnet/lib-linux-x64
cd ..
./dotnet/build.sh
```

### Build the NuGet Package

Now from either gitbash (Windows) or a WSL prompt, where ever you have the dotnet core SDK
installed, you can generate a NuGet package.

```shell
cd dotnet/src
dotnet pack -c Release
```

This NuGet package is self contained with the needed platform specific glue libraries, and
the needed DLLs for windows. That is, when using this package under windows, one does not
need to install mcsapi.so and the support libraries separately.

### Testing

Currently the tests are very basic and rely on the t1 table existing in the mcsapi database.
The easiest way to perform the tests is to run the MariaDB ColumnStore in the WSL instance, make
the mcsapi database and t1 table, and then run ```dotnet test```. If you copy over the
WSL Columnstore.xml file to e.g. /c/opt/mariadb/etc, then you can also run the tests from Windows.
You also have to ensure that libmcsapi.so and dotnet_mcsapi.so have a valid PATH entry.

### Testing on Windows

To run the tests in dotnet/test in windows, one will need to set some environment variables, which are
mostly the same as outlined in the main README for windows testing:

* ```COLUMNSTORE_XML_DIR``` -- Required for windows, points to the folder holding the etc/Columnstore.xml file.
* ```PATH``` -- This environment variable must also contain the dotnet/lib-win-x64 directory (as an absolute path).
  Windows uses this to find the DLLs to use for testing.

## END OF LINE

[//]: # (cSpell:ignore cmake gitbash mkdir libxml libuv libiconv mcsapi mcsmysql mariadb columnstore libmcsapi nuget .)
