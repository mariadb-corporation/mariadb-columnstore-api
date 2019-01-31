using System;
using System.IO;
using CommandLine;
using Microsoft.Extensions.Logging;
using MariaDB.Data.ColumnStore;

namespace MariaDB.Data.ColumnStore.ExampleLoader
{
    class Options
    {
        [Option('f', "file", HelpText = "The CSV file, a directory containing CSV files.")]
        public string DataDirectory { get; set; }

        [Option("header-lines", HelpText = "The number of lines to ignore in the csv file. Defaults to 1")]
        public int HeaderLines { get; set; } = -1; // Strange default so we can detect when it is set to zero.

        [Option('c', "config", HelpText = "Path to the Columnstore.xml file, e.g. /opt/mariadb/etc/Columnstore.xml")]
        public string ColumnStoreXmlFilePath { get; set; }

        [Option('d', "db", HelpText = "Database name.")]
        public string DbName { get; set; }

        [Option('t', "table", HelpText = "Table name to load.")]
        public string TableName { get; set; }
    }

    class Program
    {
        public static ColumnStoreDriver mcsDriver;
        public static ILoggerFactory loggerFactory;
        public static ILogger logger;

        static void Main(string[] args)
        {
            loggerFactory = new LoggerFactory().AddConsole();
            logger = loggerFactory.CreateLogger<Program>();

            Parser.Default.Settings.MaximumDisplayWidth = 132; // For help formatting.

            try
            {
                var result = Parser.Default.ParseArguments<Options>(args)
                    .WithParsed<Options>(opts => ProcessOptions(opts));
            }
            catch (Exception ex)
            {
                Console.WriteLine("* * *  EXCEPTION: " + ex.Message + "   * * *");
                Console.WriteLine(string.Empty);
                Console.WriteLine(ex.StackTrace);
            }
        }

        /// <summary>
        /// Process the options (and run the program).
        /// 
        /// This method tries to connect to the database before
        /// checking other options so that one can verify they can
        /// connect to a database via the API without having to get
        /// all the other options correct.
        /// </summary>
        public static void ProcessOptions(Options options)
        {
            var cpImport = new CpImport(
                    loggerFactory.CreateLogger<CpImport>(),
                    options.ColumnStoreXmlFilePath
                );

            // These could be null, but we check below.
            cpImport.DbName = options.DbName;
            cpImport.TableName = options.TableName;

            if (options.HeaderLines >= 0)
            {
                cpImport.HeaderLines = options.HeaderLines;
            }

            if (!cpImport.TryGetDriver())
            {
                Console.Error.WriteLine("Could not create a ColumnStoreDriver!");
                Console.Error.WriteLine("(Perhaps try --config /path/to/Columnstore.xml ?)");
                return;
            }
            else if (string.IsNullOrWhiteSpace(options.DbName))
            {
                Console.Error.WriteLine("Need a --db DATABASE");
                return;
            }
            else if (string.IsNullOrWhiteSpace(options.TableName))
            {
                Console.Error.WriteLine("Need a --table TABLE");
                return;
            }
            else if (!cpImport.TryConnect())
            {
                Console.Error.WriteLine(
                        "Failed to make a test connection to "
                        + cpImport.TableName
                        + " in "
                        + cpImport.DbName
                    );
                return;
            }
            else if (string.IsNullOrWhiteSpace(options.DataDirectory))
            {
                Console.Error.WriteLine("Need a --file file.csv");
            }
            else if (File.Exists(options.DataDirectory))
            {
                cpImport.ImportCsv(options.DataDirectory);
            }
            else if (Directory.Exists(options.DataDirectory))
            {
                cpImport.ImportCsvFilesInDirectory(options.DataDirectory);
            }
            else
            {
                // Did not exist at all.
                Console.Error.WriteLine("ERROR: {0} does not exist.", options.DataDirectory);
            }
        }
    }
}
