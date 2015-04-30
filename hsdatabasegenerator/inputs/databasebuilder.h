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
    float mLastAggroScore;
    float mLastBCScore;

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
