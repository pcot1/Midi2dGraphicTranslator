#-------------------------------------------------
#
# Project created by QtCreator 2016-09-07T16:35:54
#
#-------------------------------------------------

QT       -= core gui

TARGET = libRtMidi
TEMPLATE = lib
CONFIG += staticlib

SOURCES += RtMidi.cpp
HEADERS += RtMidi.h

LIBS += -L/C/Windows/System32 -lwinmm
#
#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
