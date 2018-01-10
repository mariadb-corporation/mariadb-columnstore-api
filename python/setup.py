from distutils.core import setup, Extension
import sys

name='_pymcsapi'
if sys.version_info[0] == 3:
    name='_pymcsapi3'

pymcsapi_module = Extension(name,
                               sources=['pymcsapi_wrap.cxx',],
                               extra_compile_args=["-D_FORTIFY_SOURCE=2", "-std=c++11", "-Wformat", "-Wformat-security", "-fstack-protector-all", "-fstack-check", "-Wno-format-truncation", "-Wno-unused-command-line-argument", "-Wno-unknown-warning-option", "-pie", "-DBUILDING_MYMCSAPI", "-O2", "-g", "-DNDEBUG", "-fPIC"],
                               )
setup (name = 'pymcsapi',
              ext_modules = [pymcsapi_module,],
              py_modules = ["pymcsapi"]
              )

#rename _pymcsapi3.xxx.so to _pymcsapi3.so
if sys.version_info[0] == 3:
    import os
    for file in os.listdir("."):
        if file.startswith("_pymcsapi3"):
            os.rename(file, '_pymcsapi3.so')
            break
    
