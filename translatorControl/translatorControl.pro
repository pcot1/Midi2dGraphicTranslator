#-------------------------------------------------
#
# Project created by QtCreator 2016-09-03T09:21:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = translatorControl
TEMPLATE = app


SOURCES += main.cpp\
        gui.cpp \
    MidiConsumer.cpp \
    MidiGraphicTranslator.cpp \
    MidiSource.cpp \
    Midivent.cpp

HEADERS  += gui.h \
    MidiConsumer.h \
    MidiGraphicTranslator.h \
    MidiSource.h \
    Midivent.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libRtMidi/release/ -llibRtMidi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libRtMidi/debug/ -llibRtMidi

INCLUDEPATH += $$PWD/../libRtMidi
DEPENDPATH += $$PWD/../libRtMidi

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libRtMidi/release/liblibRtMidi.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libRtMidi/debug/liblibRtMidi.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libRtMidi/release/libRtMidi.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libRtMidi/debug/libRtMidi.lib

LIBS += -L/C/Windows/System32 -lwinmm
