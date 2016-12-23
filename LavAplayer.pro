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

INCLUDEPATH += \
    /usr/local/include

LIBS += \
    -lz \
    -L/usr/local/lib \
    -ltag


SOURCES += main.cpp\
        mainwindow.cpp \
        tagmanager.cpp \
        customdial.cpp \
        playlistview.cpp \
        playlisttabwidget.cpp

HEADERS  += mainwindow.h \
    tagmanager.h \
    customdial.h \
    playlistview.h \
    playlisttabwidget.h \
    initdb.h \
    dboperate.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
