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
