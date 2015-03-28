#include <QCoreApplication>
#include <ctime>
#include "game.h"
#include "carddb.h"
#include "inputs/databasebuilder.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);
    if (argc < 4)
        qCritical() << "Required params: cardDB hero1_id hero2_id [seed=0]";
    QCoreApplication a(argc, argv);
    const QStringList& ARGS = QCoreApplication::arguments();
    if (argc > 4) {
        int seed = ARGS.at(4).toInt();
        if (seed < 0)
            qsrand(time(NULL));
        else
            qsrand(seed);
    }
    else
        qsrand(0);


    const QString PATH("../../hearthstone/db/");
    CardDB* const CARD_DB = CardDB::Instance();

    Game::InitializeGlobals(true);
    CARD_DB->buildCardDB(PATH + ARGS.at(1));

    // Create game with random decks and start it
    Game g("David", CARD_DB->buildHero(ARGS.at(2)), "", new DatabaseBuilder(),
           "Aaron", CARD_DB->buildHero(ARGS.at(3)), "", new DatabaseBuilder());
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
