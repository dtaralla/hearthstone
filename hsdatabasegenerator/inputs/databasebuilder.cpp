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
    mLastAggroScore(0),
    mLastBCScore(0)
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
    Game::AggroScore* aScore2 = mMe->game()->meta_AggroScore();
    Game::BoardControlScore* bScore2 = mMe->game()->meta_BoardControlScore();

    if (mLastAction != NULL && !mLastAction->isTargetedAction() && mLastAction->type() != ActionTypes::END_TURN) {
        float aScore = aScore2->score - mLastAggroScore;
        DBOutput::Instance(mMe->game(), DBOutput::AGGRO)->addEntry(mLastEnvironment, mLastAction, aScore);

        float bScore = bScore2->score - mLastBCScore;
        DBOutput::Instance(mMe->game(), DBOutput::BOARD_CONTROL)->addEntry(mLastEnvironment, mLastAction, bScore);
    }

    mLastEnvironment = mMe->game()->environment();
    mLastAggroScore = aScore2->score;
    mLastBCScore = bScore2->score;
    delete aScore2;
    delete bScore2;

    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));

    if (actions->size() == 1)
        mLastAction = actions->first();
    else {
        do {
            mLastAction = actions->at(qrand() % actions->size());

            // Only one chance on 5 to indeed play fireball (else too much samples will have this action taken)
            if (mLastAction->type() == ActionTypes::SPECIAL_POWER && actions->size() > 2 && qrand() % 3)
                mLastAction = actions->at(qrand() % actions->size());

        } while (mLastAction->type() == ActionTypes::END_TURN);
    }
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
