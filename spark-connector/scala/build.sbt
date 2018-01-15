name := "spark-scala-mcsapi-connector"

version := "0.1"

scalaVersion := "2.11.8"

libraryDependencies += "org.apache.spark" % "spark-sql_2.11" % "2.2.1"
unmanagedBase := baseDirectory.value / "../../java/build/libs"
