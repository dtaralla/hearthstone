#include "gamecontroller.h"
#include "game.h"


GameController::GameController(Game* g) : QObject(NULL),
    mWorkerThread(g)
{
    setGame(g);
}

GameController::GameController() : QObject(NULL),
    mWorkerThread(NULL)
{

}

GameController::~GameController()
{
    qDebug() << "Destroy gamecontroller";
    mWorkerThread.quit();
    mWorkerThread.wait();
}

GameController* GameController::setGame(Game* g)
{
    if (mWorkerThread.isRunning()) {
        mWorkerThread.quit();
        mWorkerThread.wait();
        disconnect(&mWorkerThread, SIGNAL(finished()), 0, 0);
        disconnect(this, SIGNAL(startGame()), 0, 0);
        mWorkerThread.setGame(g);
    }
    else if (mWorkerThread.game() == NULL) {
        mWorkerThread.setGame(g);
    }


    g->initPlayers(&mWorkerThread);
    g->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, SIGNAL(finished()), g, SLOT(deleteLater()));
    connect(this, SIGNAL(startGame()), g, SLOT(enterGameLoop()));
    connect(g, SIGNAL(finished()), this, SIGNAL(finished()));
    mWorkerThread.start();

    return this;
}

Game* GameController::game() const
{
    return mWorkerThread.game();
}


