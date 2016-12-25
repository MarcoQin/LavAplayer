#!/bin/bash
mkdir -p build
cd build;
qmake ../LavAplayer.pro
make
./LavAplayer.app/Contents/MacOS/LavAplayer
