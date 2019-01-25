Application compilation and execution
=====================================

To submit last section's sample application to Spark setup you first have to compile it into a Java archive.
We'll use `sbt`_ for this purpose, but other build tools like Maven would work as well.

Install sbt on the system which you want to use for compiling. Then create a sbt build project with following file structure:

.. code-block:: java

   ./build.sbt
   ./lib/spark-scala-mcsapi-connector.jar
   ./src/main/scala/ColumnStoreExporter.scala
   
The build file build.sbt contains the build information, the `lib` directory contains the `mcsapi for Scala` library as unmanaged dependency, and the `src/main/scala/` directory contains the actual code to be executed.

.. literalinclude:: ../../example/ExportDataFrame/build.sbt
   :caption: build.sbt

To compile the project execute:

.. code-block:: bash

   sbt package

This will build the application ``target/scala/columnstoreexporter-example_2.11-1.0.jar`` that can be copied to the Spark master to be executed.

.. code-block:: bash
   
   docker cp target/scala/columnstoreexporter-example_2.11-1.0.jar SPARK_MASTER:/root
   docker exec -it SPARK_MASTER spark-submit --master spark://master:7077 /root/columnstoreexporter-example_2.11-1.0.jar


.. _sbt: https://www.scala-sbt.org/