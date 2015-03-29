#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "gamethread.h"
class Game;

class GameController : public QObject
{
    Q_OBJECT
signals:
    void startGame();
    void finished();

public:
    GameController(Game* g);
    GameController();
    ~GameController();

    GameController* setGame(Game* g);
    Game* game() const;

private:
    GameThread mWorkerThread;
};

#endif // GAMECONTROLLER_H
