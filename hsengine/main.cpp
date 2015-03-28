#include <QApplication>
#include <QThread>
#include "game.h"
#include "carddb.h"
#include "player.h"
#include "inputs/baseHumanPlayer/logichumanplayer.h"
#include "inputs/scriptedplayer.h"
#include "inputs/aaron/valuefunctionchecker.h"
#include "inputs/aaron/databasebuilder.h"
#include "inputs/randomplayer.h"
#include <Python.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    if (argc < 7)
        qCritical() << "Required params: cardDB hero1_id hero2_id deck1.json deck2.json dbGenerationOn";
    QApplication a(argc, argv);
    const QStringList& ARGS = QGuiApplication::arguments();
    if (argc > 7) {
        int seed = ARGS.at(7).toInt();
        if (seed < 0)
            qsrand(time(NULL));
        else
            qsrand(seed);
    }
    else
        qsrand(0);


    const QString      PATH("../hearthstone-simulator/db/");
    const bool         GENERATION_MODE = ARGS.at(6).toInt() > 0;
    CardDB* const      CARD_DB = CardDB::Instance();

    Game::InitializeGlobals();
    CARD_DB->buildCardDB(PATH + ARGS.at(1));


    /*Py_SetProgramName(argv[0]);  /* optional but recommended */
    /*Py_Initialize();
    PyRun_SimpleString("from time import time,ctime\n"
                       "print 'Today is',ctime(time())\n");
    Py_Finalize();*/

    // Create IO device for players and decide which decks to use
    PlayerInput* p1Input, * p2Input;
    QString p1Deck, p2Deck;
    if (GENERATION_MODE) {
        p1Input = new DatabaseBuilder();
        p2Input = new DatabaseBuilder();
        p1Deck = p2Deck = "";
    }
    else {
        p1Input = new LogicHumanPlayer();
        p2Input = new ScriptedPlayer();
        p1Deck = PATH + ARGS.at(4);
        p2Deck = PATH + ARGS.at(5);
    }

    // Create players' heroes
    Hero* p1Hero = CARD_DB->buildHero(ARGS.at(2));
    Hero* p2Hero = CARD_DB->buildHero(ARGS.at(3));

    // Create game and start it (this creates p1 and p2 in thread "g")
    Game g("David", p1Hero, p1Deck, p1Input,
           "Aaron", p2Hero, p2Deck, p2Input);
    g.connect(&g, SIGNAL(finished()), (QCoreApplication*)&a, SLOT(quit()));
    g.start();

    return a.exec();
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n\tFrom: %s:%u, %s\n", localMsg.constData(),
                context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n\tFrom: %s:%u, %s\n", localMsg.constData(),
                context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n\tFrom: %s:%u, %s\n", localMsg.constData(),
                context.file, context.line, context.function);
        exit(-1);

    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n\tFrom: %s:%u, %s\n", localMsg.constData(),
                context.file, context.line, context.function);
        exit(-1);
    }

    fflush(stderr);
}
