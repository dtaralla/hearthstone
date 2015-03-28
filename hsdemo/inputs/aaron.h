#ifndef AARON_H
#define AARON_H

#include "playerinput.h"
#include "game_globals.h"

class Player;

class Aaron : public PlayerInput
{
public:
    Aaron(QObject* parent = NULL);
    virtual ~Aaron();
    void attachPlayer(Player* p);

private:
    Player* m_me;
    Player* m_enemy;

    // PlayerInput interface
public slots:
    void onEventTriggered(IORequest* ir);
    void askForAction(IORequest* ir);
    void askForPosition(IORequest* ir);
    void askForTarget(IORequest* ir);
};

#endif // AARON_H
