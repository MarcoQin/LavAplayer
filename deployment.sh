#!/bin/bash

cd build

mkdir -p LavAplayer.app/Contents/Library
cp -R /usr/local/opt/ffmpeg/lib/* LavAplayer.app/Contents/Library
install_name_tool -change /usr/local/opt/ffmpeg/lib/libavformat.57.dylib @load_path/../Library/libavformat.57.dylib LavAplayer.app/Contents/MacOS/LavAplayer

install_name_tool -change /usr/local/opt/ffmpeg/lib/libavformat.57.dylib @load_path/../Library/libavformat.57.dylib LavAplayer.app/Contents/MacOS/LavAplayer

install_name_tool -change /usr/local/opt/ffmpeg/lib/libavcodec.57.dylib @load_path/../Library/libavcodec.57.dylib LavAplayer.app/Contents/MacOS/LavAplayer

install_name_tool -change /usr/local/opt/ffmpeg/lib/libswscale.4.dylib @load_path/../Library/libswscale.4.dylib LavAplayer.app/Contents/MacOS/LavAplayer

install_name_tool -change /usr/local/opt/ffmpeg/lib/libavutil.55.dylib @load_path/../Library/libavutil.55.dylib LavAplayer.app/Contents/MacOS/LavAplayer

install_name_tool -change /usr/local/opt/ffmpeg/lib/libswresample.2.dylib @load_path/../Library/libswresample.2.dylib LavAplayer.app/Contents/MacOS/LavAplayer

cp /usr/local/opt/fftw/lib/libfftw3.3.dylib LavAplayer.app/Contents/Library

install_name_tool -change /usr/local/opt/fftw/lib/libfftw3.3.dylib @load_path/../Library/libfftw3.3.dylib LavAplayer.app/Contents/MacOS/LavAplayer

mkdir LavAplayer.app/Contents/Frameworks

cp -R ~/Qt5.6.2/5.6/clang_64/lib/QtWidgets.framework LavAplayer.app/Contents/Frameworks
cp -R ~/Qt5.6.2/5.6/clang_64/lib/QtGui.framework LavAplayer.app/Contents/Frameworks
cp -R ~/Qt5.6.2/5.6/clang_64/lib/QtCore.framework LavAplayer.app/Contents/Frameworks

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore LavAplayer.app/Contents/Frameworks/QtCore.framework/Versions/5/QtCore

install_name_tool -change @rpath/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore LavAplayer.app/Contents/MacOS/LavAplayer
