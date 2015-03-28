#-------------------------------------------------
#
# Project created by QtCreator 2015-02-11T09:28:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hearthstone-simulator
TEMPLATE = app

INCLUDEPATH += C:/Anaconda/include
LIBS += -LC:/Anaconda/libs -lpython27

DEFINES += QT_USE_QSTRINGBUILDER

SOURCES += main.cpp\
    game.cpp \
    cardinfo.cpp \
    spellinfo.cpp \
    characterinfo.cpp \
    player.cpp \
    card.cpp \
    spell.cpp \
    character.cpp \
    hero.cpp \
    minion.cpp \
    playerinput.cpp \
    event.cpp \
    iorequest.cpp \
    heroinfo.cpp \
    actions/action.cpp \
    actions/summonminionaction.cpp \
    actions/attackaction.cpp \
    actions/endturnaction.cpp \
    actions/addmanaaction.cpp \
    carddb.cpp \
    inputs/baseHumanPlayer/cardwidget.cpp \
    actions/specialpoweraction.cpp \
    inputs/baseHumanPlayer/guihumanplayer.cpp \
    inputs/baseHumanPlayer/logichumanplayer.cpp \
    inputs/baseHumanPlayer/logwindow.cpp \
    inputs/baseHumanPlayer/selectactiontargetdialog.cpp \
    stringable.cpp \
    actions/playaction.cpp \
    inputs/baseHumanPlayer/selectpositiondialog.cpp \
    enchantment.cpp \
    actions/destroyenchantmentaction.cpp \
    actions/damagingspellaction.cpp \
    silenceenchantment.cpp \
    actions/silencetargetaction.cpp \
    actions/enchantaction.cpp \
    actions/drawcardaction.cpp \
    actions/addarmoraction.cpp \
    triggers/trigger.cpp \
    triggers/turnendtrigger.cpp \
    triggers/minionkilledtrigger.cpp \
    triggers/characterdamagedtrigger.cpp \
    triggers/cardplayedtrigger.cpp \
    actions/summonnewminionsaction.cpp \
    expressions/target/targetexpression.cpp \
    expressions/target/targetsfromgroupexpression.cpp \
    expressions/group/groupexpression.cpp \
    expressions/target/targetthisexpression.cpp \
    actions/dealdmghealingaction.cpp \
    actions/freezeaction.cpp \
    actions/damageandfreezeaction.cpp \
    expressions/target/eventcharacterparamexpression.cpp \
    actions/targettedaction.cpp \
    inputs/randomplayer.cpp \
    inputs/scriptedplayer.cpp \
    inputs/aaron/aaron.cpp \
    inputs/aaron/valuefunctionchecker.cpp \
    inputs/aaron/databasebuilder.cpp \
    inputs/aaron/dbouput.cpp \
    game_globals.cpp

HEADERS  += \
    game.h \
    cardinfo.h \
    spellinfo.h \
    characterinfo.h \
    card.h \
    player.h \
    spell.h \
    character.h \
    hero.h \
    minion.h \
    playerinput.h \
    vptr.h \
    event.h \
    iorequest.h \
    heroinfo.h \
    actions/action.h \
    actions/summonminionaction.h \
    actions/attackaction.h \
    actions/endturnaction.h \
    actions/addmanaaction.h \
    carddb.h \
    actions/specialpoweraction.h \
    inputs/baseHumanPlayer/guihumanplayer.h \
    inputs/baseHumanPlayer/logichumanplayer.h \
    inputs/baseHumanPlayer/cardwidget.h \
    inputs/baseHumanPlayer/logwindow.h \
    inputs/baseHumanPlayer/selectactiontargetdialog.h \
    stringable.h \
    actions/playaction.h \
    inputs/baseHumanPlayer/selectpositiondialog.h \
    enchantment.h \
    actions/destroyenchantmentaction.h \
    actions/damagingspellaction.h \
    silenceenchantment.h \
    actions/silencetargetaction.h \
    actions/enchantaction.h \
    actions/drawcardaction.h \
    actions/addarmoraction.h \
    triggers/trigger.h \
    triggers/turnendtrigger.h \
    triggers/minionkilledtrigger.h \
    triggers/characterdamagedtrigger.h \
    triggers/cardplayedtrigger.h \
    actions/summonnewminionsaction.h \
    expressions/target/targetexpression.h \
    expressions/target/targetsfromgroupexpression.h \
    expressions/group/groupexpression.h \
    expressions/target/targetthisexpression.h \
    actions/dealdmghealingaction.h \
    actions/freezeaction.h \
    actions/damageandfreezeaction.h \
    expressions/target/eventcharacterparamexpression.h \
    actions/targettedaction.h \
    actions/cardactions.h \
    actions/allactions.h \
    inputs/randomplayer.h \
    inputs/scriptedplayer.h \
    inputs/aaron/aaron.h \
    inputs/aaron/valuefunctionchecker.h \
    inputs/aaron/databasebuilder.h \
    inputs/aaron/dbouput.h \
    game_globals.h

FORMS    += \
    inputs/baseHumanPlayer/cardwidget.ui \
    inputs/baseHumanPlayer/guihumanplayer.ui \
    inputs/baseHumanPlayer/logwindow.ui \
    inputs/baseHumanPlayer/selectactiontargetdialog.ui \
    inputs/baseHumanPlayer/selectpositiondialog.ui

RESOURCES += \
    res/res.qrc

DISTFILES += \
    inputs/aaron/trainClassifier.py
