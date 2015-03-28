#include "endturnaction.h"
#include "game.h"
#include <QtDebug>

EndTurnAction::EndTurnAction(Game* g) :
    mGame(g)
{

}

EndTurnAction::~EndTurnAction()
{

}



void EndTurnAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    mGame->nextTurn();
}

Action* EndTurnAction::cloneFlyweight() const
{
    return new EndTurnAction(mGame);
}


QString EndTurnAction::toString() const
{
    return "End of turn";
}

ActionType EndTurnAction::type() const
{
    return ActionTypes::END_TURN;
}

Action* EndTurnAction::setSourceCard(Card* c) const
{
    Q_UNUSED(c);
    qCritical() << "SourceCard on a EndTurnAction?!";
    return NULL;
}
