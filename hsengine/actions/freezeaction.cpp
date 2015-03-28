#include "freezeaction.h"
#include "expressions/target/targetexpression.h"
#include "character.h"
#include "enchantment.h"
#include "destroyenchantmentaction.h"
#include "triggers/turnendtrigger.h"

FreezeAction::FreezeAction(int id, const QSharedPointer<TargetExpression>& target) :
    TargetedAction(id, target)
{

}

FreezeAction::~FreezeAction()
{
}



QString FreezeAction::toString() const
{
    return "Freeze " + m_target->toString();
}

ActionType FreezeAction::type() const
{
    return ActionTypes::SET_FROZEN;
}

void FreezeAction::resolve(const Event* e)
{
    Q_CHECK_PTR(m_card);

    QVector<Character*>* targets = m_targets(m_card, e);

    foreach (Character* t, *targets) {
        t->freeze(m_card);
    }

    m_refreshLastTargets(targets);
    delete targets;
}

FreezeAction* FreezeAction::cloneFlyweight() const
{
    return new FreezeAction(m_id, m_target);
}
