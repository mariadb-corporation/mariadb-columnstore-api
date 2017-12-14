# CpImport Java Example
This is a basic example of how to implement cpimport using the MariaDB ColumnStore API Java Wrapper.

# Building
This example requires the MariaDB ColumnStore API Java Wrapper to be installed. 

To build the application use following command:
```sh
$ ./gradelw build
```

It builds the application, executes basic tests and packs all required dependencies in a single runnable jar file, which can be found in ./build/libs/CpImport-all.jar.

# Execution
To start cpimport use following command:

```sh
$ java -jar ./build/libs/CpImport-all.jar
```

Required arguments are the target database, the target table and the import file.
CpImport is able to interpret ambiguous date and dateTime formats in Java's SimpleDateFormat notation with two optional command line parameters.

The import file needs to be csv encoded with the delimiter |.

# Troubleshooing
You might be required to manually link the MariaDB ColumnStore API Java Wrapper's 'lib_javamcsapi.so' to your operating system's lib directory. 

