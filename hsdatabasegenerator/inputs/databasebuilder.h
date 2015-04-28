#ifndef DATABASEBUILDER_H
#define DATABASEBUILDER_H

#include "playerinput.h"
#include <QVector>

class Action;
class Character;

class DatabaseBuilder : public PlayerInput
{
public:
    DatabaseBuilder();
    virtual ~DatabaseBuilder();

private:
    Player* mMe;
    Action* mLastAction;
    QVector<float> mLastEnvironment;
    QVector<float> mLastAggroEnvironment;
    float mLastAggroScore;

    // PlayerInput interface
public:
    void attachPlayer(Player *p);

public slots:
    void onEventTriggered(IORequest* ir);
    void askForAction(IORequest* ir);
    void askForPosition(IORequest* ir);
    void askForTarget(IORequest* ir);
};

#endif // DATABASEBUILDER_H
