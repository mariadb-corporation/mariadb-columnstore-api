

using System;
using Microsoft.Extensions.Logging;
using CsvHelper;
using System.IO;
using System.Diagnostics;

namespace MariaDB.Data.ColumnStore
{
    public class CpImport
    {
        private ILogger _logger;
        public ColumnStoreDriver McsDriver { get; private set; }
        public string ColumnStoreXmlFilepath { get; private set; }
        public string DbName { get; set; }
        public string TableName { get; set; }

        /// <summary>
        /// The number of lines to ignore before processing the data.
        /// </summary>
        public int HeaderLines { get; set; } = 1;

        public CpImport(ILogger logger)
        {
            this._logger = logger;
            this._logger?.LogInformation("New CpImport w/o a specific XML file.");
        }

        /// <summary>
        /// New CpImport object with a logger and config file specified.
        /// </summary>
        /// <param name="logger">Logger for messages, or set to null.</param>
        /// <param name="columnStoreXmlFilepath">The full path and filename to the Colmunstore.xml file.</param>
        public CpImport(ILogger logger, string columnStoreXmlFilepath)
        {
            this._logger = logger;
            this.ColumnStoreXmlFilepath = columnStoreXmlFilepath;
            if (string.IsNullOrWhiteSpace(this.ColumnStoreXmlFilepath))
            {
                this._logger?.LogInformation("New CpImport w/o a specific XML file.");
            }
            else
            {
                this._logger?.LogInformation("New CpImport w/Columnstore.xml = " + this.ColumnStoreXmlFilepath);
            }
        }


        public bool TryGetDriver()
        {
            try
            {
                if (string.IsNullOrWhiteSpace(this.ColumnStoreXmlFilepath))
                {
                    this.McsDriver = new ColumnStoreDriver();
                }
                else
                {
                    this.McsDriver = new ColumnStoreDriver(this.ColumnStoreXmlFilepath);
                }
            }
            catch (Exception e)
            {
                this._logger?.LogError("Failed to setup ColumnStoreDriver: " + e.Message);
                if (string.IsNullOrWhiteSpace(this.ColumnStoreXmlFilepath))
                {
                    this._logger?.LogError("(Perhaps specify the ColumnStore.xml full path?");
                }
                return false;
            }
            return true;
        }

        /// <summary>
        /// Try to connect to the ColumnStore database through the API.
        /// Useful for checking the configuration is valid
        /// before attempting an import.
        /// </summary>
        /// <returns>Returns true if a connection could be made.</returns>
        public bool TryConnect()
        {
            if (string.IsNullOrWhiteSpace(this.DbName))
            {
                this._logger?.LogError("No DbName Set! (Therefore cannot connect.)");
                return false;
            }
            else if (string.IsNullOrWhiteSpace(this.TableName))
            {
                this._logger?.LogError("No TableName Set! (Therefore cannot connect.)");
            }
            else
            {
                ColumnStoreBulkInsert bulkInsert = null;
                try
                {
                    bulkInsert = this.McsDriver.CreateBulkInsert(this.DbName, this.TableName, 0, 0);
                    this._logger?.LogInformation("Connected to " + this.TableName + " in " + this.DbName);
                }
                catch (Exception e)
                {
                    this._logger?.LogError("Could not connect: " + e.Message);
                    return false;
                }
                finally
                {
                    // Otherwise this check will hang.
                    if (!(bulkInsert is null))
                    {
                        bulkInsert.Rollback();
                        bulkInsert = null;
                    }
                }
            }
            return true;
        }

        /// <summary>
        /// Load all *.csv files in a directory.
        /// </summary>
        public void ImportCsvFilesInDirectory(string directoryName)
        {
            foreach (var file in Directory.GetFiles(directoryName, "*.csv"))
            {
                // Note that file contains a full path.
                this.ImportCsv(file);
            }
        }


        /// <summary>
        /// Load a single file.
        /// </summary>
        public void ImportCsv(string fileName)
        {
            using (var streamReader = File.OpenText(fileName))
            {
                this.ImportCsv(streamReader);
            }
        }


        /// <summary>
        /// Load CSV data from a TextReader.
        /// 
        /// This performs the entire commit in a single transaction after
        /// reading the entire file.
        /// </summary>
        public void ImportCsv(TextReader textReader)
        {
            // https://joshclose.github.io/CsvHelper/reading#getting-all-records
            var csv = new CsvReader(textReader);
            csv.Configuration.MissingFieldFound = null; // Don't throw when missing.

            for (int i = 0; i < this.HeaderLines; i++)
            {
                csv.Read();
                csv.ReadHeader();
            }

            var systemCatalog = this.McsDriver.GetSystemCatalog();
            var tableInfo = systemCatalog.GetTable(this.DbName, this.TableName);

            // Avoid a method call in the loop.
            ushort tableColumnCount = tableInfo.GetColumnCount();


            var stopwatch = new Stopwatch();
            stopwatch.Start();

            ColumnStoreBulkInsert bulkInsert = null;
            try
            {
                bulkInsert = this.McsDriver.CreateBulkInsert(this.DbName, this.TableName, 0, 0);

                int count = 0;
                while (csv.Read())
                {
                    count += 1;
                    if (count % 1000 == 0)
                    {
                        this._logger?.LogInformation("{0} Rows in {1} seconds.", count, stopwatch.Elapsed);
                    }

                    // Note the use of ushort here.
                    for (ushort i = 0; i < tableColumnCount; i++)
                    {
                        // The API does not like nulls, so null-collesse to the empty string.
                        bulkInsert.SetColumn(i, csv.GetField(i) ?? string.Empty);
                    }
                    bulkInsert.WriteRow();
                }
            }
            catch (Exception ex)
            {
                this._logger?.LogError("Exception Thrown: " + ex.Message);

                if (!(bulkInsert is null))
                {
                    bulkInsert.Rollback();
                    bulkInsert = null;
                }

                throw; // And re-throw the exception
            }

            stopwatch.Stop();
            this._logger?.LogInformation("Total Time: {0}", stopwatch.Elapsed);

            if (!(bulkInsert is null))
            {
                bulkInsert.Commit();
                // print a short summary of the insertion process
                ColumnStoreSummary summary = bulkInsert.GetSummary();
                this._logger?.LogInformation("Execution time: " + summary.GetExecutionTime());
                this._logger?.LogInformation("Rows inserted: " + summary.GetRowsInsertedCount());
                this._logger?.LogInformation("Truncation count: " + summary.GetTruncationCount());
                this._logger?.LogInformation("Saturated count: " + summary.GetSaturatedCount());
                this._logger?.LogInformation("Invalid count: " + summary.GetInvalidCount());
            }
        }
    }
}