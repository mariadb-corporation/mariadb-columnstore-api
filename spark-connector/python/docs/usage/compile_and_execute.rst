Application execution
=====================

To submit last section's sample application to your Spark setup you simply have to copy it to the Spark master and execute it though `spark-submit`.

.. code-block:: bash
   
   docker cp ExportDataFrame.py SPARK_MASTER:/root
   docker exec -it SPARK_MASTER spark-submit --master spark://master:7077 /root/ExportDataFrame.py
