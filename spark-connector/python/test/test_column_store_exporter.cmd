IF [%2]==[] (
  echo %0 python_executable_path pyspark_driver_python
  exit /b
)

set PYSPARK_PYTHON=%1
set PYSPARK_DRIVER_PYTHON=%2
%1 -m pytest %~dp0test_column_store_exporter.py
if %ERRORLEVEL% neq 0 ( exit /b )
%1 -m pytest %~dp0test_column_store_SQL_generation.py
if %ERRORLEVEL% neq 0 ( exit /b )
