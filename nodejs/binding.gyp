{
  "targets": [
    {
      "target_name": "nodemcsapi",
      "sources": [ "nodemcsapi_wrap.cxx" ],
      "libraries": [ "./../../src/libmcsapi.so" ],
      "cflags": ["-std=c++11"]
    }
  ]
}
