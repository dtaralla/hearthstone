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
    mLastAction(NULL),
    mLastAggroScore(0)
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
        DBOutput::Instance(mMe->game(), DBOutput::BOARD_CONTROL)->addEntry(mLastEnvironment, mLastAction, s->score);
        delete s;

        Game::AggroScore* aScore = mMe->game()->meta_AggroScore();
        float s2 = aScore->score - mLastAggroScore;
        delete aScore; aScore = NULL;
        DBOutput::Instance(mMe->game(), DBOutput::AGGRO)->addEntry(mLastAggroEnvironment, mLastAction, s2);
    }

    Game::AggroScore* aScore = mMe->game()->meta_AggroScore();
    mLastAggroScore = aScore->score;
    delete aScore; aScore = NULL;
    mLastEnvironment = mMe->game()->environment();
    mLastAggroEnvironment = mMe->game()->environment();

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
