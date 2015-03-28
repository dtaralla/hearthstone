#include "databasebuilder.h"
#include "character.h"
#include "actions/action.h"
#include "dbouput.h"
#include "iorequest.h"
#include "player.h"
#include "game.h"

DatabaseBuilder::DatabaseBuilder() :
    PlayerInput(),
    mMe(NULL),
    mLastAction(NULL)
{
}

DatabaseBuilder::~DatabaseBuilder()
{

}



void DatabaseBuilder::attachPlayer(Player *p)
{
    mMe = p;
}

void DatabaseBuilder::onEventTriggered(IORequest *ir)
{
    Q_UNUSED(ir);
}

void DatabaseBuilder::askForAction(IORequest *ir)
{
    if (mLastAction != NULL && !mLastAction->isTargetedAction() && mLastAction->type() != ActionTypes::END_TURN) {
        Game::BoardControlScore* s = mMe->game()->meta_BoardControlScore();
        DBOutput::Instance(mMe->game())->addEntry(mLastEnvironment, mLastAction, s->score);
        delete s;
    }

    mLastEnvironment = mMe->game()->environment();
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));
    mLastAction = actions->at(qrand() % actions->size());
    ir->setResponse(mLastAction);
}

void DatabaseBuilder::askForPosition(IORequest *ir)
{
    ir->setResponse(new int(0));
}

void DatabaseBuilder::askForTarget(IORequest *ir)
{
    QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));
    ir->setResponse(targets->at(qrand() % targets->size()));
}
