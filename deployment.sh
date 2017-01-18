#!/bin/bash

cd build

mkdir -p LavAplayer.app/Contents/Frameworks

cp -R /Library/Frameworks/SDL2.framework LavAplayer.app/Contents/Frameworks

install_name_tool -id @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 LavAplayer.app/Contents/Frameworks/SDL2.framework/Versions/A/SDL2
install_name_tool -change @rpath/SDL2.framework/Versions/A/SDL2 @executable_path/../Frameworks/SDL2.framework/Versions/A/SDL2 LavAplayer.app/Contents/MacOS/LavAplayer

macdeployqt LavAplayer.app -dmg
