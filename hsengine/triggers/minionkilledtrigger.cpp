#include "minionkilledtrigger.h"
#include "event.h"
#include "minion.h"
#include "player.h"

MinionKilledTrigger::MinionKilledTrigger(QVector<Action*>* actions, Owner owningPlayer, CharacterType minionType) :
    Trigger(actions),
    m_owningPlayer(owningPlayer),
    m_minionType(minionType)
{

}

MinionKilledTrigger::MinionKilledTrigger(Action* action, Owner owningPlayer, CharacterType minionType) :
    Trigger(action),
    m_owningPlayer(owningPlayer),
    m_minionType(minionType)
{

}

MinionKilledTrigger::~MinionKilledTrigger()
{

}



bool MinionKilledTrigger::listensTo(const Event& e) const
{
    Minion* killedMinion = VPtr<Minion>::AsPtr(e.extra("dyingMinion"));
    if (m_owningPlayer == Owners::ALLY && killedMinion->owner() != m_card->owner())
        return false;

    if (m_owningPlayer == Owners::ENEMY &&
        killedMinion->owner() != m_card->owner()->opponent())
        return false;

    return m_minionType & killedMinion->characterType();
}

Trigger* MinionKilledTrigger::clone() const
{
    return new MinionKilledTrigger(m_actions, m_owningPlayer, m_minionType);
}
