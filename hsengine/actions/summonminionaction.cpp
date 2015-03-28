#include "summonminionaction.h"
#include "game.h"
#include "player.h"
#include "minion.h"
#include "card.h"


SummonMinionAction::SummonMinionAction(Minion* c) :
    Action()
{
    m_card = c;
}

void SummonMinionAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    int pos = m_card->owner()->selectPosition((Minion*) m_card);
    m_card->owner()->game()->summonMinion((Minion*) m_card, pos);
}

Action* SummonMinionAction::cloneFlyweight() const
{
    return new SummonMinionAction((Minion*) m_card);
}

bool SummonMinionAction::resolvable(const Event* e) const
{
    Q_UNUSED(e);
    return m_card->owner()->minions()->size() < 7;
}


QString SummonMinionAction::toString() const
{
    return "Summon a " + m_card->name();
}

ActionType SummonMinionAction::type() const
{
    return ActionTypes::SUMMON;
}

Action* SummonMinionAction::setSourceCard(Card* c) const
{
    qCritical() << "Not meant to be called; card was" << *c->base();
    return NULL;
}
