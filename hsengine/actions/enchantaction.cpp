#include "enchantaction.h"
#include "enchantment.h"
#include "hero.h"
#include "player.h"
#include "expressions/target/targetexpression.h"

EnchantAction::EnchantAction(int id, const QSharedPointer<TargetExpression>& target,
                             const QSharedPointer<Enchantment>& e) :
    TargetedAction(id, target),
    m_enchantment(e)
{
}

EnchantAction::~EnchantAction()
{
    m_enchantment.clear();
}

const Enchantment* EnchantAction::enchantment() const
{
    return m_enchantment.data();
}



QString EnchantAction::toString() const
{
    return "Enchant " + m_target->toString() + ": " + m_enchantment->toString();
}

ActionType EnchantAction::type() const
{
    return ActionTypes::ENCHANT_CHARACTER;
}

void EnchantAction::resolve(const Event* e)
{
    Q_CHECK_PTR(m_card);
    QVector<Character*>* targets = m_targets(m_card, e);
    foreach (Character* c, *targets) {
        c->addEnchantment(m_enchantment->specialize(m_card, c));
    }

    m_refreshLastTargets(targets);
    delete targets;
}

EnchantAction* EnchantAction::cloneFlyweight() const
{
    return new EnchantAction(m_id, m_target, m_enchantment);
}
