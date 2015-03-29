#include "gamethread.h"

GameThread::GameThread(Game* g) :
    mGame(g)
{

}

GameThread::~GameThread()
{

}

void GameThread::setGame(Game* g)
{
    mGame = g;
}

Game* GameThread::game() const
{
    return mGame;
}

