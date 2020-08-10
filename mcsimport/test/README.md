# MariaDB ColumnStore Remote CpImport - test suite

Remote CpImport's test suite can either be executed manually via

```
test.py MCSIMPORT_EXECUTABLE
```

or through cmake's regression test suite. 

The test suite is written in Python and successively executes the tests of this sub-directories.

In order to function properly the Python modules mentioned in requirements.txt need to be installed prior testing.
```shell
# e.g. via pip from this test directory
pip2 install -r requirements.txt
```

By default the test suite uses the default Columnstore.xml residing in `/etc/columnstore/Columnstore.xml` for the bulk injection and a JDBC connection with user `root` on `127.0.0.1` without password to execute DDL.  
This is fine for local ColumnStore installations on Linux. On Windows and for remote tests these parameters need to be overwritten through the environment variables `COLUMNSTORE_XML_DIR`, `MCSAPI_CS_TEST_IP`, `MCSAPI_CS_TEST_USER`, and `MCSAPI_CS_TEST_PASSWORD`.

## Test definition
Here a short summary how a test can be designed.

### Test files
| File                               | Purpose                                                             | Required |
| ---------------------------------- |:-------------------------------------------------------------------:|:--------:|
| prepare.py                         | generate synthetic data for test and verification                   | NO       |
| DDL.sql                            | prepare the necessary tables on the remote columnstore instance     | NO       |
| config.yaml                        | states mcsimport's command line parameters and expected return code | YES      |
| mapping.yaml                       | contains the mapping between csv and columnstore columns            | NO       |
| Columnstore.xml                    | ColumnStore configuration for an alternative target                 | NO       |
| input.csv                          | this file is injected into columnstore via mcsimport                | NO       |
| expected.csv                       | contains the expected results of the injection in csv format        | NO       |

### Test procedure
1) read the `config.yaml` and extract the test's name, expected return code, and mcsimport command line parameters
2) if `prepare.py` is found execute its `prepare_test()` method to create the required files (e.g. `input.csv`, `expected.csv`, `DDL.sql`, ...)
3) if `DDL.sql` is found forward its instructions to the remote columnstore instance to prepare the test environment
4) execute mcsimport according to its instructions in `config.yaml` and validate its return code
5) if `expected.csv` is found validate that its number of rows matches the count(*) of rows in the columnstore table. Further validate row by row if the injected values match the expected. For large injections the test validation coverage can be adjusted with the optional test configuration parameter `validation_coverage`.
6) if `prepare.py` is found execute its `cleanup_test()` method to clean up the generated files (useful for large injections)

**NOTE**  
In order to validate the results via `expected.csv`, the first column of the target table needs to be labelled as `id` with distinct values as row identifiers.

## Tests
Here a list of executed tests by the test suite and short explanation.

| Name                                    | Tests                                                                                               | Folder             |
| --------------------------------------- |:----------------------------------------------------------------------------------------------------|:------------------:|
| all datatype boundary                   | boundary tests for every datatype if it was injected (incl. NULL)                                   | boundary           |
|  1,000,000 row load                     | load test if 1,000,000 rows of synthetic data can be injected (and compare random 2% with expected) | load               |
| 50,000,000 row load                     | load test if 50,000,000 rows of synthetic (int) data can be injected (only comparing # of rows)     | load_2             |
| implicit csv > columnstore              | mapping test for the case that there are more csv columns than columnstore columns                  | implicit_csv_cs    |
| implicit columnstore > csv              | mapping test for the case that there are more columnstore columns than csv columns [fail]           | implicit_cs_csv    |
| implicit default_non_mapped             | mapping test for above test case with additional default_non_mapped option                          | implicit_df_non_mp |
| mapping implicit                        | mapping test for an implicit mapping file that shuffles the order of csv and cs columns             | mapping_implicit_1 |
| mapping implicit - ignore               | mapping test for implicit mapping file (like above) with used ignore tag                            | mapping_implicit_2 |
| mapping implicit columnstore > csv      | mapping test for implicit mapping file where there are more cs than csv columns [fail]              | mapping_implicit_3 |
| mapping implicit cs > csv def_non_map   | mapping test like above with additional default_non_mapped option                                   | mapping_implicit_4 |
| mapping implicit - target definition    | mapping test for implicit mapping file that tests specified default values via target definition    | mapping_implicit_5 |
| mapping explicit                        | mapping test for an explicit mapping file that shuffles the order of csv and cs columns             | mapping_explicit_1 |
| mapping explicit - ignore               | mapping test for an explicit mapping file and csv columns > cs columns (implicitly ignoring)        | mapping_explicit_2 |
| mapping explicit - wrong target name    | mapping test for explicit mapping stating a wrong target column name [fail]                         | mapping_explicit_3 |
| mapping explicit - def_non_map          | mapping test for explicit mapping, implicit ignores, unmapped targets, and default_non_map option   | mapping_explicit_4 |
| mapping explicit - target definition    | mapping test for explicit mapping file that tests specified default values via target definition    | mapping_explicit_5 |
| mapping explicit - map csv column twice | mapping test for explicit mapping file that maps a csv column twice to different columnstore colums | mapping_explicit_6 |
| implicit - global date-format           | mapping test for a global date format                                                               | date_format_1      |
| mapping implicit - global date-format   | mapping test using an implicit mapping file and global date format                                  | date_format_2      |
| mapping explicit - global date-format   | mapping test using an explicit mapping and global date format                                       | date_format_3      |
| mapping implicit - column date-format   | mapping test using an implicit mapping file and column specific date formats                        | date_format_4      |
| mapping explicit - column date-format   | mapping test using an explicit mapping file and column specific date formats                        | date_format_5      |
| mapping implicit - both date-formats    | mapping test using an implicit mapping file and column specific and global date formats             | date_format_6      |
| mapping explicit - both date-formats    | mapping test using an explicit mapping file and columns specific and global date formats            | date_format_7      |
| seperator implicit                      | seperator test without mapping file [seperator \| ]                                                 | seperator_impl     |
| seperator mapping implicit              | seperator test with implicit mapping file [seperator ; ]                                            | seperator_map_impl |
| seperator mapping explicit              | seperator test with an explicit mapping file [seperator : ]                                         | seperator_map_expl |
| mapping implicit - both default opt     | default option test for implicit mapping file and global and target specific default values         | default_map_impl   |
| mapping explicit - both default opt     | default option test for explicit mapping file and global and target specific default values         | default_map_expl   |
| windows input file line endings         | tests that CRLF line endings are supported for input files                                          | windows_line_ends  |
| special combination implicit inject     | test using a complex implicit mapping file and global parameter flags                               | special_1          |
| special combination explicit inject     | test using a complex explicit mapping file and global parameter flags                               | special_2          |
| special combination hybrid inject       | test using a complex hybrid of implicit and later explicit mapping, defaults and global paramters   | special_3          |
| alternative columnstore configuration   | tests the usage of an alternative columnstore configuration submitted via command line parameter    | alt_columstore_con |
| enclosesure test with default parameter | tests the injection with the default enclosing and escaping character (")                           | enclose_1          |
| encl. test with special escape char.    | tests the injection with the default enclosing char (") and special escape char (/)                 | enclose_2          |
| encl. test with special encl. char.     | tests the injection with a special enclosing char (+) and default escape char (")                   | enclose_3          |
| encl. test with sp encl. and esc. char. | tests the injection with a special enclosing char (+) and special escape char (\\)                  | enclose_4          |
| test with sp encl., esc. char. and deli | tests the injection with a special enclosing char (+), escape char (\\) and delimiter (\|)          | enclose_5          |
| tests if the header line is ignored     | tests if the header line is ignored when chosen by option                                           | header_1           |
| tests of the null option -n 1           | tests if NULL strings are identified correctly as NULL values                                       | null_option_1      |
| mcsimport fail without ig_malformed_csv | tests if mcsimport fails without the ignore_malformed_csv command line option                       | malformed_csv_1    |
| mcsimport succeed with ig_malformed_csv | tests if mcsimport succeeds with the ignore_malformed_csv command line option                       | malformed_csv_2    |
| bool "true" value text injection        | tests if texts with the value "true" are transformed to 1 in numeric target columns                 | bool_transformation|
