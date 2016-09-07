#-------------------------------------------------
#
# Project created by QtCreator 2016-09-03T09:18:15
#
#-------------------------------------------------

QT       -= core gui

TARGET = RtMidi
TEMPLATE = lib

DEFINES += RTMIDI_LIBRARY

SOURCES += RtMidi.cpp

HEADERS += RtMidi.h\
        rtmidi_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
