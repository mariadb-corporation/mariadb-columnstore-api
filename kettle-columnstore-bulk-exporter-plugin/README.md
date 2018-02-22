# MariaDB ColumnStore - Pentaho Data Integration - Bulk Loader Plugin
This provides the source files for MariaDB's ColumunStore bulk loader plugin.

### Building the plugin from source
To build the plugin from source execute following command:
```shell
./gradlew plugin
```
**NOTE** Java SDK 8 or higher is required.

### Installation of the plugin in PDI / Kettle
Following steps are necessary to install the ColumnStore bulk loader plugin.
1. build the plugin from source
2. extract the archive _kettle-columnstore-bulk-exporter-plugin-*.zip_ from _build/distributions/_ into your PDI installation directory _$PDI-INSTALLATION/plugins_
3. copy/link the library _libjavamcsapi.so_ (from [MariaDB ColumnStore API Java Wrapper](../java/)) into PDI's java.library.path. (e.g. *$PDI-INSTALLATION/libswt/linux/x86_64*)

### Configuration
The plugin tries to use ColumnStore's default configuration _/usr/local/mariadb/columnstore/etc/Columnstore.xml_.

Individual configurations can be assigned within each block.

### Limitations
The plugin currently can't handle blob datatypes and only supports multi inputs to one block if the input field names are equal for all input sources.

### Testing
To test the plugin you can execute the job _test.kjb_ from the _test_ directory.

### Known issues
#### Pentaho Data Integration 8 - java.library.path
Pentaho Data Integration 8 uses a relative java.library.path which isn't compatible with plugins using JNI (cf. [PDI-17038](https://jira.pentaho.com/browse/PDI-17038)). To get our plugin up and running you have to manually set the java.library.path to an absolute path in spoon.sh.

Necessary changes in the community version around line 234:
```shell
#JNI needs the absolute path as java.library.path
ABSOLUTE_LIBPATH=`pwd`/launcher/$LIBPATH

OPT="$OPT $PENTAHO_DI_JAVA_OPTIONS -Dhttps.protocols=TLSv1,TLSv1.1,TLSv1.2 -Djava.library.path=$ABSOLUTE_LIBPATH -DKETTLE_HOME=$KETTLE_HOME -DKETTLE_REPOSITORY=$KETTLE_REPOSITORY -DKETTLE_USER=$KETTLE_USER -DKETTLE_PASSWORD=$KETTLE_PASSWORD -DKETTLE_PLUGIN_PACKAGES=$KETTLE_PLUGIN_PACKAGES -DKETTLE_LOG_SIZE_LIMIT=$KETTLE_LOG_SIZE_LIMIT -DKETTLE_JNDI_ROOT=$KETTLE_JNDI_ROOT"
```

Necessary changes in the enterprise version around line 258:
```shell
#JNI needs the absolute path as java.library.path
ABSOLUTE_LIBPATH=`pwd`/launcher/$LIBPATH

OPT="$OPT $PENTAHO_DI_JAVA_OPTIONS -Dhttps.protocols=TLSv1,TLSv1.1,TLSv1.2 -Djava.library.path=$ABSOLUTE_LIBPATH -DKETTLE_HOME=$KETTLE_HOME -DKETTLE_REPOSITORY=$KETTLE_REPOSITORY -DKETTLE_USER=$KETTLE_USER -DKETTLE_PASSWORD=$KETTLE_PASSWORD -DKETTLE_PLUGIN_PACKAGES=$KETTLE_PLUGIN_PACKAGES -DKETTLE_LOG_SIZE_LIMIT=$KETTLE_LOG_SIZE_LIMIT -DKETTLE_JNDI_ROOT=$KETTLE_JNDI_ROOT -Dpentaho.installed.licenses.file=$PENTAHO_INSTALLED_LICENSE_PATH"
```

#### Renaming of columns of datatype TEXT
ColumnStore currently doesn't support the renaming of columns of type TEXT (cf. [MCOL-1228](https://jira.mariadb.org/browse/MCOL-1228)). As a result our plugin can't convert data types CHAR, VARCHAR, and STRING into TEXT if more space is required.
