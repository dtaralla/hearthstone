#-------------------------------------------------
#
# Project created by QtCreator 2015-03-28T16:21:49
#
#-------------------------------------------------

QT       -= gui

TARGET = hsengine
TEMPLATE = lib

DEFINES += HSENGINE_LIBRARY \
           QT_USE_QSTRINGBUILDER

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

SOURCES += game.cpp \
    player.cpp \
    card.cpp \
    spell.cpp \
    character.cpp \
    hero.cpp \
    minion.cpp \
    playerinput.cpp \
    event.cpp \
    iorequest.cpp \
    actions/action.cpp \
    actions/summonminionaction.cpp \
    actions/attackaction.cpp \
    actions/endturnaction.cpp \
    actions/addmanaaction.cpp \
    carddb.cpp \
    actions/specialpoweraction.cpp \
    stringable.cpp \
    actions/playaction.cpp \
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
    game_globals.cpp \
    dbouput.cpp \
    gamecontroller.cpp \
    gamethread.cpp \
    actions/targetedaction.cpp \
    triggers/characterhealedtrigger.cpp \
    triggers/characterattackstrigger.cpp \
    cardidentity.cpp \
    heroidentity.cpp \
    spellidentity.cpp \
    characteridentity.cpp

HEADERS += game.h \
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
    actions/action.h \
    actions/summonminionaction.h \
    actions/attackaction.h \
    actions/endturnaction.h \
    actions/addmanaaction.h \
    carddb.h \
    actions/specialpoweraction.h \
    stringable.h \
    actions/playaction.h \
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
    actions/cardactions.h \
    actions/allactions.h \
    game_globals.h \
    dbouput.h \
    gamecontroller.h \
    gamethread.h \
    actions/targetedaction.h \
    triggers/characterhealedtrigger.h \
    triggers/characterattackstrigger.h \
    cardidentity.h \
    characteridentity.h \
    heroidentity.h \
    spellidentity.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    page__card_database_format.dox
