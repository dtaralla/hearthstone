#include "action.h"
#include <stdlib.h>

Action::Action() :
    m_card(NULL)
{

}

Action::~Action()
{
}

Card* Action::source() const
{
    return m_card;
}

bool Action::deletesItselfAfterResolve() const
{
    return false;
}

bool Action::resolvable(const Event* e) const
{
    Q_UNUSED(e);
    return true;
}

Action* Action::setSourceCard(Card* c) const
{
    Action* toRet = cloneFlyweight();
    toRet->m_card = c;
    return toRet;
}

void Action::addSpellDmg(int quantity)
{
    Q_UNUSED(quantity);
}

bool Action::isTargetedAction() const
{
    return false;
}
