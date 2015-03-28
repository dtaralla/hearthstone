#include "turnendtrigger.h"
#include "card.h"
#include "player.h"

TurnEndTrigger::TurnEndTrigger(QVector<Action*>* actions, Owner playerEndingTurn) :
    Trigger(actions),
    m_playerEndingTurn(playerEndingTurn)
{

}

TurnEndTrigger::TurnEndTrigger(Action* action, Owner playerEndingTurn) :
    Trigger(action),
    m_playerEndingTurn(playerEndingTurn)
{

}

TurnEndTrigger::~TurnEndTrigger()
{

}



bool TurnEndTrigger::listensTo(const Event& e) const
{
    Q_CHECK_PTR(m_card);
    if (m_playerEndingTurn == Owners::ANY_OWNER)
        return true;

    Player* endingTurnPlayer = VPtr<Player>::AsPtr(e.extra("player"));
    if (m_playerEndingTurn & Owners::ALLY)
        return endingTurnPlayer == m_card->owner();

    //if (m_playerEndingTurn & Owners::ENEMY)
    return endingTurnPlayer == m_card->owner()->opponent();
}

Trigger* TurnEndTrigger::clone() const
{
    return new TurnEndTrigger(m_actions, m_playerEndingTurn);
}
