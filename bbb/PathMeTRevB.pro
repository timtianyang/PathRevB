#-------------------------------------------------
#
# Project created by QtCreator 2014-12-15T17:03:30
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = PathMeTRevB
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CFLAGS = -fpermissive
QMAKE_CXXFLAGS = -fpermissive
QMAKE_LFLAGS = -fpermissive
SOURCES += main.cpp \
    C920.cpp \
    flashdriver_helper.cpp \
    GPIOConst.cpp \
    GPIOManager.cpp \
    bbbserver.cpp \
    SpiDriver.cpp \
    sensor_interrupt.cpp

HEADERS += \
    C920.h \
    flashdriver_helper.h \
    GPIOConst.h \
    GPIOManager.h \
    bbbserver.h \
    SpiDriver.h \
    sensor_interrupt.h \
    Sleeper.h

target.files = PathMeTRevB
target.path = /home/root
INSTALLS = target
INCLUDEPATH += /usr/local/include



