#-------------------------------------------------
#
# Project created by QtCreator 2015-02-11T09:28:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hsdemo
TEMPLATE = app

win32:INCLUDEPATH += C:/Anaconda/include
win32:LIBS += -LC:/Anaconda/libs -lpython27
unix:INCLUDEPATH += /usr/include/python2.7
unix:LIBS += -lpython2.7 -lpthread -ldl  -lutil -lm

DEFINES += QT_USE_QSTRINGBUILDER

FORMS    += \
    inputs/baseHumanPlayer/cardwidget.ui \
    inputs/baseHumanPlayer/guihumanplayer.ui \
    inputs/baseHumanPlayer/logwindow.ui \
    inputs/baseHumanPlayer/selectactiontargetdialog.ui \
    inputs/baseHumanPlayer/selectpositiondialog.ui \
    inputs/baseHumanPlayer/qlogtext.ui

RESOURCES += \
    res/res.qrc

HEADERS += \
    inputs/randomplayer.h \
    inputs/scriptedplayer.h \
    inputs/baseHumanPlayer/cardwidget.h \
    inputs/baseHumanPlayer/guihumanplayer.h \
    inputs/baseHumanPlayer/logichumanplayer.h \
    inputs/baseHumanPlayer/logwindow.h \
    inputs/baseHumanPlayer/selectactiontargetdialog.h \
    inputs/baseHumanPlayer/selectpositiondialog.h \
    consoleprogressbar.h \
    gamethreadpool.h \
    inputs/nora.h

SOURCES += \
    inputs/randomplayer.cpp \
    inputs/scriptedplayer.cpp \
    main.cpp \
    inputs/baseHumanPlayer/cardwidget.cpp \
    inputs/baseHumanPlayer/guihumanplayer.cpp \
    inputs/baseHumanPlayer/logichumanplayer.cpp \
    inputs/baseHumanPlayer/logwindow.cpp \
    inputs/baseHumanPlayer/selectactiontargetdialog.cpp \
    inputs/baseHumanPlayer/selectpositiondialog.cpp \
    consoleprogressbar.cpp \
    gamethreadpool.cpp \
    inputs/nora.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../hsengine/release/ -lhsengine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../hsengine/debug/ -lhsengine
else:unix: LIBS += -L$$OUT_PWD/../hsengine/ -lhsengine

INCLUDEPATH += $$PWD/../hsengine
DEPENDPATH += $$PWD/../hsengine
