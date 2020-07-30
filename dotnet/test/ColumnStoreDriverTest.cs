using System;
using System.IO;
using Xunit;
using MariaDB.Data.ColumnStore;

// cSpell:ignore xunit mariadb mcsapi libuv libiconv

namespace test
{
    public class ColumnStoreDriverTest
    {
        // https://stackoverflow.com/questions/5116977/how-to-check-the-os-version-at-runtime-e-g-windows-or-linux-without-using-a-con
        public static bool IsLinux
        {
            get
            {
                int p = (int)Environment.OSVersion.Platform;
                return (p == 4) || (p == 6) || (p == 128);
            }
        }



        public static string[] ColumnstoreXmlPaths =
            new string[]
            {
                "/etc",
                "/etc/columnstore",
            };

        public static string TestDb = "mcsapi";
        public static string TestTable = "t1";

        public static string ColumnStoreXmlFilename = "Columnstore.xml";
        public static string ColumnStoreInstallEnvVariable = "COLUMNSTORE_INSTALL_DIR";

        /// <summary>
        /// The path and filename found for the Columnstore.xml config.
        /// </summary>
        public static string ColumnstoreXmlFilePath = null;

        public static bool FoundMcsapiDll = false;
        public static bool FoundDotnetMcsapiDll = false;
        public static bool FoundLibiconvDll = false;
        public static bool FoundLibuvDll = false;
        public static bool FoundLibxml2Dll = false;

        static ColumnStoreDriverTest()
        {
            var columnstoreInstallDir = Environment.GetEnvironmentVariable(ColumnStoreInstallEnvVariable);
            if (string.IsNullOrWhiteSpace(columnstoreInstallDir))
            {
                for (int i = 0; i < ColumnstoreXmlPaths.Length; i++)
                {
                    if (File.Exists(Path.Combine(ColumnstoreXmlPaths[i], ColumnStoreXmlFilename)))
                    {

                        ColumnstoreXmlFilePath = Path.Combine(ColumnstoreXmlPaths[i], ColumnStoreXmlFilename);
                        break;
                    }
                }
            }
            else if (!Directory.Exists(columnstoreInstallDir))
            {
                throw new ArgumentException(
                        "The path specified in the environment variable "
                        + ColumnStoreInstallEnvVariable
                        + " ("
                        + columnstoreInstallDir
                        + ") does not exist!"
                        + " (Please be sure to specify an absolute path.)"
                    );
            }
            else
            {
                ColumnstoreXmlFilePath = Path.Combine(columnstoreInstallDir, "etc", ColumnStoreXmlFilename);
            }

            if (string.IsNullOrWhiteSpace(ColumnstoreXmlFilePath))
            {
                throw new ArgumentException(
                        "Did not find "
                        + ColumnStoreXmlFilename
                        + " Please set the environment variable "
                        + ColumnStoreInstallEnvVariable
                        + " to a directory containing this file."
                    );
            }
            else if (!File.Exists(ColumnstoreXmlFilePath))
            {
                throw new ArgumentException(
                        ColumnstoreXmlFilePath
                        + " does not exist? Please check/set the environment variable "
                        + ColumnStoreInstallEnvVariable
                    );
            }

            if (IsLinux)
            {

            }
            else
            {
                // Windows
                if (!PathContainsWindowsDlls())
                {
                    throw new ArgumentException(
                        "Did not find all of the needed DLLs in your PATH."
                        + " SOURCE_DIR/dotnet/lib-win-x64 needs to be in the PATH"
                        + " and contain:"
                        + " dotnet_mcsapi.dll mcsapi.dll libiconv.dll libuv.dll libxml2.dll"
                    );
                }
            }
        }

        // public static string GetExecutingDirectoryName()
        // {
        //     var location = new Uri(Assembly.GetEntryAssembly().GetName().CodeBase);
        //     return new FileInfo(location.AbsolutePath).Directory.FullName;
        // }


        /// <summary>
        /// Crude way to check for needed DLLs. For windows, the DLLs need to be
        /// in the PATH. They cannot be in the same directory as the .NET DLLs as
        /// the c++ DLLs are unmanaged and putting them in the .NET assembly directory
        /// will produce hard-to-diagnose errors.
        /// </summary>
        public static bool PathContainsWindowsDlls()
        {
            var currentPaths = Environment.GetEnvironmentVariable("PATH").Split(Path.PathSeparator);
            for (int i = 0; i < currentPaths.Length; i++)
            {
                if (currentPaths[i].EndsWith(@"\dotnet\lib-win-x64") || currentPaths[i].EndsWith("/dotnet/lib-win-x64"))
                {

                    foreach (string filePath in Directory.EnumerateFiles(currentPaths[i], "*.dll"))
                    {
                        switch (Path.GetFileName(filePath).ToLower())
                        {
                            case "dotnet_mcsapi.dll":
                                FoundDotnetMcsapiDll = true;
                                break;
                            case "mcsapi.dll":
                                FoundMcsapiDll = true;
                                break;
                            case "libiconv.dll":
                                FoundLibiconvDll = true;
                                break;
                            case "libuv.dll":
                                FoundLibuvDll = true;
                                break;
                            case "libxml2.dll":
                                FoundLibxml2Dll = true;
                                break;
                        }
                    }
                }
            }
            return FoundDotnetMcsapiDll && FoundMcsapiDll
                && FoundLibiconvDll && FoundLibuvDll && FoundLibxml2Dll;
        }


        /// <summary>
        /// Very basic test. This just makes sure that we can load the
        /// dlls/shared libraries by creating a new instance.
        /// 
        /// e.g., run:
        /// 
        ///  dotnet test --filter "FullyQualifiedName=test.ColumnStoreDriverTest.DllLoadTest"
        /// 
        /// To verify the setup is OK for runing further tests.
        /// </summary>
        [Fact]
        public void DllLoadTest()
        {
            var d = new ColumnStoreDriver(ColumnstoreXmlFilePath);
            Assert.NotNull(d);
        }

        /// <summary>
        /// This test goes one step further by calling createBulkInsert, which
        /// will use the information in Columntore.xml to connect to the node(s)
        /// and check the DB + tables exist.
        /// </summary>
        [Fact]
        public void ConnectTest()
        {
            var d = new ColumnStoreDriver(ColumnstoreXmlFilePath);
            Assert.NotNull(d);
            ColumnStoreBulkInsert b = null;
            try
            {
                b = d.CreateBulkInsert(TestDb, TestTable, 0, 0);
                Assert.NotNull(b);
                b.Rollback(); // important, otherwise this will hang and throw.
            }
            catch (Exception ex)
            {
                Console.WriteLine(string.Empty);
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
                Console.WriteLine(string.Empty);
                if (ex.Message.Contains("not found"))
                {
                    Console.WriteLine("Please make sure the table " + TestTable
                        + " exists in the " + TestDb + " datbase.");
                    Console.WriteLine("Please create the 't1' table outlined in https://mariadb.com/kb/en/library/columnstore-bulk-write-sdk/");
                }
                else if (ex.Message.Contains("Table already locked"))
                {
                    Console.WriteLine("Try: sudo dbrmctl resume");
                }
                Assert.True(false, "Could not connect/create a bulk insert instance.");
            }
            finally
            {
                if (!(b is null))
                {
                    b.Rollback();
                }
            }
        }

        /// <summary>
        /// Make sure that connecting to a non-existent db fails.
        /// </summary>
        [Fact]
        public void FailConnectTest()
        {
            var d = new ColumnStoreDriver(ColumnstoreXmlFilePath);
            Assert.NotNull(d);
            ColumnStoreBulkInsert b = null;
            try
            {
                b = d.CreateBulkInsert("db-does-not-exist", TestTable, 0, 0);
                Assert.True(false, "Created a bulk insert instance against a non-existent db.");
            }
            catch (Exception ex)
            {
                if (ex.Message.Contains("not found"))
                {
                    Assert.True(true, "Could not connect/create a bulk insert instance against a non-existent db.");
                }
                else
                {
                    Assert.True(false, "Wrong exception returned? " + ex.Message);
                }
            }
            finally
            {
                if (!(b is null))
                {
                    b.Rollback();
                }
            }
        }


        /// <summary>
        /// Make sure the column catalog information is correct.
        /// </summary>
        [Fact]
        public void ColumnCatalogTest()
        {
            var d = new ColumnStoreDriver(ColumnstoreXmlFilePath);
            Assert.NotNull(d);
            try
            {
                var tableInfo = d.GetSystemCatalog().GetTable(TestDb, TestTable);
                Assert.NotNull(tableInfo);

                // Note that the number in GetColumn is a ushort, so beware when
                // looping with an int.
                Assert.Equal("i", tableInfo.GetColumn(0).GetColumnName());
                Assert.Equal(ColumnstoreDataTypesT.DATATYPEINT, tableInfo.GetColumn(0).GetType());

                Assert.Equal("c", tableInfo.GetColumn(1).GetColumnName());
                Assert.Equal(ColumnstoreDataTypesT.DATATYPECHAR, tableInfo.GetColumn(1).GetType());

                // One can also get information by name.
                Assert.Equal((ushort)0, tableInfo.GetColumn("i").GetPosition());
                Assert.Equal(ColumnstoreDataTypesT.DATATYPEINT, tableInfo.GetColumn("i").GetType());

            }
            catch (Exception ex)
            {
                Assert.True(false, "Getting table information failed: " + ex.Message);
            }
        }
    }
}
