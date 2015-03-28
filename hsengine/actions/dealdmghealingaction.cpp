#include "dealdmghealingaction.h"
#include "card.h"
#include "player.h"
#include "minion.h"
#include "hero.h"
#include "expressions/target/targetexpression.h"

DealDmgHealingAction::DealDmgHealingAction(int id, const QSharedPointer<TargetExpression>& target, int quantity) :
    DamagingSpellAction(id, target, quantity)
{
}

DealDmgHealingAction::~DealDmgHealingAction()
{
}

ActionType DealDmgHealingAction::type() const
{
    return (m_quantity < 0) ? ActionTypes::HEAL : ActionTypes::DEAL_DMG;
}



void DealDmgHealingAction::resolve(const Event* e)
{
    Q_CHECK_PTR(m_card);
    QVector<Character*>* targets = m_targets(m_card, e);
    foreach (Character* c, *targets) {
        c->owner()->dealDamageHealing(m_card, c, -m_quantity);
    }

    m_refreshLastTargets(targets);
    delete targets;
}

DealDmgHealingAction* DealDmgHealingAction::cloneFlyweight() const
{
    return new DealDmgHealingAction(m_id, m_target, m_quantity);
}

QString DealDmgHealingAction::toString() const
{
    if (m_quantity < 0)
        return QString("Heal %1 damage on ").arg(-m_quantity) + m_target->toString();
    else
        return QString("Deal %1 damage on ").arg(m_quantity) + m_target->toString();
}

