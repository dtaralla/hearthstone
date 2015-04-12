/**
 * @file hsdatabasegenerator/main.cpp
 *
 * Contains the main program executed by the Hearthstone Simulator Database
 * Generator.
 *
 * @sa hsdatabasegenerator
 */

/**
 * @defgroup hsdatabasegenerator Hearthstone Simulator Database Generator
 *
 * @brief This module regroups the program and classes needed to build
 * databases of [environment + action + board control score] samples.
 */

#include <QCoreApplication>
#include <ctime>
#include "game.h"
#include "carddb.h"
#include "inputs/databasebuilder.h"
#include "gamethreadpool.h"
#include "consoleprogressbar.h"
#include <iostream>

#ifdef QT_NO_DEBUG
#define PATH "../../../hearthstone/db/"
#else
#define PATH "../../hearthstone/db/"
#endif


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    std::cout << "\nHearthstone Simulator Database utility (c) David Taralla, ULg\n";

    qInstallMessageHandler(myMessageOutput);
    if (argc < 6) {
        std::cout << "Usage: cardDB hero1_id hero2_id howManyGames maxThreads [seed=0]";
        return -2;
    }
    QCoreApplication a(argc, argv);
    const QStringList& ARGS = QCoreApplication::arguments();
    if (argc > 6) {
        int seed = ARGS.at(6).toInt();
        if (seed < 0)
            qsrand(time(NULL));
        else
            qsrand(seed);
    }
    else
        qsrand(0);


    CardDB* const CARD_DB = CardDB::Instance();

    Game::InitializeGlobals(true);
    CARD_DB->buildCardDB(PATH + ARGS.at(1));

    std::cout << "CMD: Simulate " << ARGS.at(4).toStdString()
              << " games, with at most " << ARGS.at(5).toStdString()
              << " concurrent jobs\n";
    QThread consoleThread;
    ConsoleProgressBar::Instance()->moveToThread(&consoleThread);
    consoleThread.start(QThread::HighPriority);

    GameThreadPool pool(ARGS.at(2), ARGS.at(3), ARGS.at(4).toInt(), ARGS.at(5).toInt());
    pool.connect(&pool, SIGNAL(progress(int)), ConsoleProgressBar::Instance(), SLOT(update(int)));
    pool.connect(&pool, SIGNAL(finished()), &consoleThread, SLOT(quit()));
    pool.connect(&pool, SIGNAL(finished()), (QCoreApplication*)&a, SLOT(quit()));

    pool.start();

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
