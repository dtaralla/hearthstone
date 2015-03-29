#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <QThread>
class Game;

class GameThread : public QThread
{
    Q_OBJECT
public:
    GameThread(Game* g);
    ~GameThread();

    void setGame(Game* g);
    Game* game() const;

private:
    Game* mGame;
};

#endif // GAMETHREAD_H
