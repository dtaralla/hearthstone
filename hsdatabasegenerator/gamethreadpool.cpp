#include "gamethreadpool.h"
#include "game.h"
#include "carddb.h"
#include "inputs/databasebuilder.h"
#include "gamecontroller.h"
#include <QCoreApplication>

GameThreadPool::GameThreadPool(const QString& p1Hero, const QString& p2Hero,
                               int howMany, uint maxThreadCount, QObject* parent) :
    QThread(parent),
    mP1Hero(p1Hero),
    mP2Hero(p2Hero),
    mHowMany(howMany),
    mSem(maxThreadCount),
    mExitCondition(0)
{
    mPool.reserve(maxThreadCount);
    for (uint i = 0; i < maxThreadCount; i += 1) {
        GameController* gc = new GameController();
        connect(gc, SIGNAL(finished()), this, SLOT(threadFinished()), Qt::DirectConnection);
        mPool.append(gc);
    }
}

GameThreadPool::~GameThreadPool()
{
    qDeleteAll(mPool);
}

void GameThreadPool::run()
{
    int remaining = mHowMany;
    while (remaining-- > 0) {
        mSem.acquire();
        DatabaseBuilder* p1Input = new DatabaseBuilder();
        DatabaseBuilder* p2Input = new DatabaseBuilder();
        p1Input->moveToThread(thread()); // Moves p1Input in QCoreApp thread
        p2Input->moveToThread(thread()); // Moves p2Input in QCoreApp thread
        Game* g = new Game("Player 1", CardDB::Instance()->buildHero(mP1Hero), "", p1Input,
                           "Player 2", CardDB::Instance()->buildHero(mP2Hero), "", p2Input);
        mPool.takeFirst()->setGame(g)->startGame();
    }

    mExitCondition.acquire(mHowMany); // Wait for all jobs to finish
}

void GameThreadPool::threadFinished()
{
    float percent = (mExitCondition.available() + 1) / ((float) mHowMany);
    emit progress((int) (100 * percent));

    mPool.append((GameController*) sender());
    mSem.release();
    mExitCondition.release();
}

