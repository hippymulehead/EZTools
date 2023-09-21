#!/bin/bash

./build.sh
make package
sudo dnf -y remove eztools
sudo dnf -y install ./eztools-*.rpm
cp ./eztools-*.rpm ~/packages
