#include "destroyenchantmentaction.h"
#include "enchantment.h"
#include "character.h"

DestroyEnchantmentAction::DestroyEnchantmentAction(Enchantment* toDestroy) :
    Action(),
    m_toDestroy(toDestroy)
{

}

DestroyEnchantmentAction::~DestroyEnchantmentAction()
{

}

ActionType DestroyEnchantmentAction::type() const
{
    return ActionTypes::DESTROY_ENCHANTMENT;
}

void DestroyEnchantmentAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    m_toDestroy->buffedCard()->removeEnchantment(m_toDestroy);
    delete m_toDestroy;
}

Action* DestroyEnchantmentAction::cloneFlyweight() const
{
    return new DestroyEnchantmentAction(m_toDestroy);
}

QString DestroyEnchantmentAction::toString() const
{
    return "Destroy enchantment of " + m_toDestroy->buffedCard()->toString() +
            " '" + m_toDestroy->name() + ": " + m_toDestroy->toString() + "'";
}

bool DestroyEnchantmentAction::deletesItselfAfterResolve() const
{
    return true;
}
