#include <QApplication>
#include <QThread>
#include "game.h"
#include "carddb.h"
#include "player.h"
#include "inputs/baseHumanPlayer/logichumanplayer.h"
#include "inputs/scriptedplayer.h"
#include "inputs/randomplayer.h"
#include "inputs/aaron.h"
#include "gamecontroller.h"
#include <Python.h>
#include <iostream>
#include "consoleprogressbar.h"
#include "gamethreadpool.h"

#ifdef QT_NO_DEBUG
#define PATH "../../hearthstone/db/"
#else
#define PATH "../../hearthstone/db/"
#endif

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    if (argc < 7)
        qCritical() << "Required params: cardDB hero1_id hero2_id deck1.json deck2.json player2Type[in 0 1 2 3] [seed=0]\nOR\n"
                    << "cardDB hero1_id hero2_id deck1.json deck2.json -1 howManyGames maxThreads [seed=0]";
    QApplication a(argc, argv);

    const QStringList& ARGS = QApplication::arguments();
    CardDB* const      CARD_DB = CardDB::Instance();

    Game::InitializeGlobals(false);
    CARD_DB->buildCardDB(PATH + ARGS.at(1));

    if (ARGS.at(6).toInt() == -1) {
        if (argc > 9) {
            int seed = ARGS.at(9).toInt();
            if (seed < 0)
                qsrand(time(NULL));
            else
                qsrand(seed);
        }
        else
            qsrand(0);

        std::cout << "CMD: Simulate " << ARGS.at(7).toStdString()
                  << " games, with at most " << ARGS.at(8).toStdString()
                  << " concurrent jobs\n";
        QThread consoleThread;
        ConsoleProgressBar::Instance()->moveToThread(&consoleThread);
        consoleThread.start(QThread::HighPriority);

        GameThreadPool pool(ARGS.at(2), ARGS.at(3), PATH + ARGS.at(4), PATH + ARGS.at(5), ARGS.at(7).toInt(), ARGS.at(8).toInt());
        pool.connect(&pool, SIGNAL(progress(int)), ConsoleProgressBar::Instance(), SLOT(update(int)));
        pool.connect(&pool, SIGNAL(finished()), &consoleThread, SLOT(quit()));
        pool.connect(&pool, SIGNAL(finished()), (QCoreApplication*)&a, SLOT(quit()));

        pool.start();
        return a.exec();
    }
    else {
        if (argc > 7) {
            int seed = ARGS.at(7).toInt();
            if (seed < 0)
                qsrand(time(NULL));
            else
                qsrand(seed);
        }
        else
            qsrand(0);

        // Create IO device for players and decide which decks to use
        PlayerInput* p1Input = new LogicHumanPlayer();
        PlayerInput* p2Input;
        switch (ARGS.at(6).toInt()) {
            case 0:
            default:
                p2Input = new Aaron();
                break;

            case 1:
                p2Input = new LogicHumanPlayer();
                break;

            case 2:
                p2Input = new RandomPlayer();
                break;

            case 3:
                p2Input = new ScriptedPlayer();
                break;
        }

        Game* g = new Game(
            "David", CARD_DB->buildHero(ARGS.at(2)), PATH + ARGS.at(4), p1Input,
            "Aaron", CARD_DB->buildHero(ARGS.at(3)), PATH + ARGS.at(5), p2Input);
        GameController game(g);
        game.connect(&game, SIGNAL(finished()), (QCoreApplication*)&a, SLOT(quit()));
        game.startGame();
        return a.exec();
    }

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
