#include "addmanaaction.h"
#include "card.h"
#include "player.h"

AddManaAction::AddManaAction(Owner target, int quantity, bool persistent) :
    Action(),
    m_target(target),
    m_quantity(quantity),
    m_persistent(persistent)
{

}

AddManaAction::~AddManaAction()
{

}

int AddManaAction::quantity() const
{
    return m_quantity;
}

Owner AddManaAction::targetOwner() const
{
    return m_target;
}

QString AddManaAction::toString() const
{
    if (m_persistent)
        return QString("Permanently add %1 mana crystal(s) to " + Owners::toString(m_target)).arg(m_quantity);

    return QString("Add %1 mana crystal(s) to " + Owners::toString(m_target)).arg(m_quantity);
}

ActionType AddManaAction::type() const
{
    return ActionTypes::ADD_MANA;
}



void AddManaAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    Q_CHECK_PTR(m_card);

    if (m_target & Owners::ALLY) {
        if (m_persistent)
            m_card->owner()->modifyMaxMana(m_quantity);
        else
            m_card->owner()->modifyCurrentMana(m_quantity);
    }

    if (m_target & Owners::ENEMY) {
        if (m_persistent)
            m_card->owner()->opponent()->modifyMaxMana(m_quantity);
        else
            m_card->owner()->opponent()->modifyCurrentMana(m_quantity);
    }
}

Action* AddManaAction::cloneFlyweight() const
{
    return new AddManaAction(m_target, m_quantity, m_persistent);
}
