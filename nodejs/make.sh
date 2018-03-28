#!/bin/sh

swig -javascript -node -c++ -I..  nodemcsapi.i
node-gyp configure build
