#-------------------------------------------------
#
# Project created by QtCreator 2016-12-19T16:30:01
#
#-------------------------------------------------

QT       += core gui\
            widgets\
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


SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/tagmanager.cpp \
    src/customdial.cpp \
    src/playlistview.cpp \
    src/playlisttabwidget.cpp \
    LAVA/lava.cpp \
    src/floatwidget.cpp \
    src/titlebar.cpp \
    src/spectrograph.cpp \
    src/spectrumanalyser.cpp \
    src/spectrumlineleft.cpp \
    src/spectrumlineright.cpp \
    src/dboperate.cpp \
    src/playstatedelegate.cpp

HEADERS  += \
    src/mainwindow.h \
    src/tagmanager.h \
    src/customdial.h \
    src/playlistview.h \
    src/playlisttabwidget.h \
    src/initdb.h \
    src/dboperate.h \
    LAVA/lava.h \
    src/floatwidget.h \
    src/titlebar.h \
    src/spectrograph.h \
    src/spectrumanalyser.h \
    src/spectrumlineleft.h \
    src/spectrumlineright.h \
    src/playstatedelegate.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
