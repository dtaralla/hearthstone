#include "gamethreadpool.h"
#include "game.h"
#include "carddb.h"
#include "inputs/aaron.h"
#include "inputs/randomplayer.h"
#include "gamecontroller.h"
#include <QCoreApplication>

GameThreadPool::GameThreadPool(const QString& p1Hero, const QString& p2Hero,
                               const QString& p1Deck, const QString& p2Deck,
                               int howMany, uint maxThreadCount,
                               bool againstScriptedPlayer, QObject* parent) :
    QThread(parent),
    mAgainstScriptedPlayer(againstScriptedPlayer),
    mP1Hero(p1Hero),
    mP2Hero(p2Hero),
    mP1Deck(p1Deck),
    mP2Deck(p2Deck),
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
        PlayerInput* p1Input = new Aaron(true);//new RandomPlayer(true);
        PlayerInput* p2Input;
        if (mAgainstScriptedPlayer)
            p2Input = new ScriptedPlayer();
        else
            p2Input = new RandomPlayer();
        p1Input->moveToThread(thread()); // Moves p1Input in QCoreApp thread
        p2Input->moveToThread(thread()); // Moves p2Input in QCoreApp thread
        Game* g = new Game("Player 1", CardDB::Instance()->buildHero(mP1Hero), mP1Deck, p1Input,
                           "Player 2", CardDB::Instance()->buildHero(mP2Hero), mP2Deck, p2Input);
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

