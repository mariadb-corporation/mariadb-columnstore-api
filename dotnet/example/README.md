# Getting Started with c# and .NET Core

This is a data loader inspired by
[MariaDB Analytics Tutorial: 5 Steps to Get Started in 10 Minutes](https://mariadb.com/resources/blog/mariadb-analytics-tutorial-5-steps-to-get-started-in-10-minutes/)
and also
<https://github.com/mariadb-corporation/mariadb-columnstore-api/tree/master/example/CpImport>

## Example

From a git-bash command line:

```shell
 dotnet run -- --config /c/opt/mariadb/etc/Columnstore.xml --db loans --table loanstats --file ~/data/LoanStats3a.csv --header-lines 2
 ```
