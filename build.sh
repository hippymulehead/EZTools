#!/bin/bash

rm *.deb
rm *.rpm
rm -rf CMakeCache.txt
rm -rf _CMake*
cmake -D$1=ON .
make clean
make
