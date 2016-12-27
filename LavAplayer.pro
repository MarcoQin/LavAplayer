#-------------------------------------------------
#
# Project created by QtCreator 2016-12-19T16:30:01
#
#-------------------------------------------------

QT       += core gui\
            sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = LavAplayer
TEMPLATE = app

QMAKE_LFLAGS += -F/Library/Frameworks/

INCLUDEPATH += \
    /usr/local/include \

LIBS += \
    -lz \
    -lavformat -lavcodec -lswscale -lavutil -lswresample\
    -framework sdl2 \
    -L/usr/local/lib \
    -ltag \
    -lfftw3 \


SOURCES += main.cpp\
        mainwindow.cpp \
        tagmanager.cpp \
        customdial.cpp \
        playlistview.cpp \
        playlisttabwidget.cpp \
        LAVA/lava.cpp \
    floatwidget.cpp \
    titlebar.cpp

HEADERS  += mainwindow.h \
    tagmanager.h \
    customdial.h \
    playlistview.h \
    playlisttabwidget.h \
    initdb.h \
    dboperate.h \
    LAVA/lava.h \
    floatwidget.h \
    titlebar.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
