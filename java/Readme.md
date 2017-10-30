# MariaDB ColumnStore API Java Wrapper
This provides a Java wrapper around the C++ MariaDB ColumnStore API. It takes advantage of [SWIG](http://www.swig.org) to provide an almost identical interface to the C++ SDK but in python.

# Building
SWIG and Java JDK 8 must be installed on the server and in the path. 
Gradle is used for java building and testing and this will be installed as needed as part of the build. Gradle will pull down the necessary java libraries for testing.


To build requires enabling the cmake option JAVA from the mariadb-columnstore-api directory:
```sh
$ cmake -DJAVA=on
$ make
```

# Testing
The unit tests can be run from the java directory:
```sh
$ ./gradlew test
```

Alternatively "make test" will run the Java tests along with the C++ API tests.

