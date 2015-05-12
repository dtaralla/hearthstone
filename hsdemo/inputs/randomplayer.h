#ifndef RANDOMPLAYER_H
#define RANDOMPLAYER_H

#include "playerinput.h"

class Player;

class RandomPlayer : public PlayerInput
{
public:
    RandomPlayer(bool writeResult=false);
    virtual ~RandomPlayer();

    // PlayerInput interface
public slots:
    virtual void attachPlayer(Player* p);

    void onGameEnded(IORequest* ir);
    void onEventTriggered(IORequest* ir);
    void askForAction(IORequest* ir);
    void askForPosition(IORequest* ir);
    void askForTarget(IORequest* ir);

private:
    Player* mMe;
    bool mWriteResults;
};

#endif // RANDOMPLAYER_H
