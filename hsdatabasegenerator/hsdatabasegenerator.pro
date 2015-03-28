#-------------------------------------------------
#
# Project created by QtCreator 2015-03-28T16:42:04
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = hsdatabasegenerator
#CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    inputs/databasebuilder.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../hsengine/release/ -lhsengine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../hsengine/debug/ -lhsengine
else:unix: LIBS += -L$$OUT_PWD/../hsengine/ -lhsengine

INCLUDEPATH += ../hsengine
DEPENDPATH += $$PWD/../hsengine

HEADERS += \
    inputs/databasebuilder.h
