#include "addarmoraction.h"
#include "hero.h"
#include "player.h"
#include "iorequest.h"

AddArmorAction::AddArmorAction(int quantity, Owner target) :
    Action(),
    m_quantity(quantity),
    m_targetOwner(target)
{

}

Owner AddArmorAction::targetOwner() const
{
    return m_targetOwner;
}

int AddArmorAction::quantity() const
{
    return m_quantity;
}



QString AddArmorAction::toString() const
{
    return QString("Add %1 armor to ").arg(m_quantity) + Owners::toString(m_targetOwner);
}

ActionType AddArmorAction::type() const
{
    return ActionTypes::ADD_ARMOR;
}

void AddArmorAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    Q_CHECK_PTR(m_card);
    if (m_targetOwner & Owners::ALLY)
        m_card->owner()->hero()->addArmor(m_quantity);

    if (m_targetOwner & Owners::ENEMY)
        m_card->owner()->opponent()->hero()->addArmor(m_quantity);
}

AddArmorAction* AddArmorAction::cloneFlyweight() const
{
    return new AddArmorAction(m_quantity, m_targetOwner);
}
