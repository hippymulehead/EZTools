#!/bin/bash

rm *.deb
rm *.rpm
rm -rf CMakeCache.txt
rm -rf _CMake*
cmake .
make clean
make
