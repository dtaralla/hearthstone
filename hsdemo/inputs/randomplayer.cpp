#include "randomplayer.h"
#include "actions/action.h"
#include "vptr.h"
#include "iorequest.h"
#include <QVector>
#include "character.h"

RandomPlayer::RandomPlayer() :
    PlayerInput(NULL)
{

}

RandomPlayer::~RandomPlayer()
{

}

void RandomPlayer::attachPlayer(Player* p)
{
    Q_UNUSED(p);
}



void RandomPlayer::onEventTriggered(IORequest* ir)
{
    Q_UNUSED(ir);
}

void RandomPlayer::askForAction(IORequest* ir)
{
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));
    ir->setResponse(actions->at(qrand() % actions->size()));
}

void RandomPlayer::askForPosition(IORequest* ir)
{
    ir->setResponse(new int(0));
}

void RandomPlayer::askForTarget(IORequest* ir)
{
    QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));
    ir->setResponse(targets->at(qrand() % targets->size()));
}
