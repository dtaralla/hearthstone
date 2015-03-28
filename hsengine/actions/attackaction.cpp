#include "attackaction.h"
#include "player.h"
#include "card.h"
#include "game.h"
#include "character.h"


AttackAction::AttackAction(Character* c) :
    Action(),
    m_lastTarget(NULL)
{
    m_card = c;
}

QString AttackAction::toString() const
{
    return "Make " + m_card->name() + " attack";
}

ActionType AttackAction::type() const
{
    return ActionTypes::ATTACK;
}

void AttackAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    QVector<Character*>* targets = m_card->owner()->game()->attackableTargets((Character*) m_card);
    m_lastTarget = m_card->owner()->selectTarget(targets, this);
    delete targets;

    m_card->owner()->executeAttack((Character*) m_card, m_lastTarget);
}

AttackAction* AttackAction::cloneFlyweight() const
{
    return new AttackAction((Character*) m_card);
}

Character* AttackAction::lastTarget() const
{
    return m_lastTarget;
}

int AttackAction::id() const
{
    return 0;
}


Action* AttackAction::setSourceCard(Card* c) const
{
    qCritical() << "Not meant to be called; card was" << c->toString();
    return NULL;
}

bool AttackAction::isTargetedAction() const
{
    return true;
}
