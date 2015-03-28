#include "damagingspellaction.h"

DamagingSpellAction::DamagingSpellAction(int id, const QSharedPointer<TargetExpression>& target, int quantity) :
    TargetedAction(id, target),
    m_quantity(quantity)
{

}

DamagingSpellAction::~DamagingSpellAction()
{

}

void DamagingSpellAction::addSpellDmg(int quantity)
{
    Q_ASSERT(m_quantity >= 0); // This function is only for damaging spells

    m_quantity += quantity;
    if (m_quantity < 0) // Cannot turn into a healing spell!
        m_quantity = 0;
}

int DamagingSpellAction::quantity() const
{
    return m_quantity;
}

