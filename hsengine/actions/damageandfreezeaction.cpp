#include "damageandfreezeaction.h"
#include "freezeaction.h"
#include "character.h"

DamageAndFreezeAction::DamageAndFreezeAction(int id, const QSharedPointer<TargetExpression>& target, int quantity) :
    DealDmgHealingAction(id, target, quantity),
    m_freezeAction(NULL)
{

}

DamageAndFreezeAction::~DamageAndFreezeAction()
{
    if (m_freezeAction != NULL)
        delete m_freezeAction;
}



QString DamageAndFreezeAction::toString() const
{
    return DealDmgHealingAction::toString() + " and Freeze it";
}

ActionType DamageAndFreezeAction::type() const
{
    return ActionTypes::FREEZE_AFTER_DMG;
}

void DamageAndFreezeAction::resolve(const Event* e)
{
    DealDmgHealingAction::resolve(e); // this updates m_lastTargets

    if (m_freezeAction == NULL)
        m_freezeAction = (FreezeAction*) FreezeAction(-1, m_target).setSourceCard(m_card);

    // If the damage killed the target, this won't be in m_lastTargets!
    if (!m_lastTargets.empty()) {
        m_freezeAction->forceTargets(m_lastTargets);
        m_freezeAction->resolve(e);
    }
}

DamageAndFreezeAction* DamageAndFreezeAction::cloneFlyweight() const
{
    return new DamageAndFreezeAction(m_id, m_target, m_quantity);
}

