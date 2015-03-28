#include "drawcardaction.h"
#include "card.h"
#include "player.h"

DrawCardAction::DrawCardAction(int quantity, Owner targetOwner) :
    Action(),
    m_quantity(quantity),
    m_targetOwner(targetOwner)
{
    Q_ASSERT(m_quantity > 0);
}

DrawCardAction::~DrawCardAction()
{

}

int DrawCardAction::quantity() const
{
    return m_quantity;
}

Owner DrawCardAction::targetOwner() const
{
    return m_targetOwner;
}



QString DrawCardAction::toString() const
{
    return Owners::toString(m_targetOwner) + QString(" draws %1 cards").arg(m_quantity);
}

ActionType DrawCardAction::type() const
{
    return ActionTypes::DRAW_CARD;
}

void DrawCardAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    Q_CHECK_PTR(m_card);

    Player* pl = m_card->owner();
    Player* oppPl = m_card->owner()->opponent();
    for (int i = 0; i < m_quantity; i += 1) {
        if (m_targetOwner & Owners::ALLY)
            pl->drawCard();
        if (m_targetOwner & Owners::ENEMY)
            oppPl->drawCard();
    }
}

Action* DrawCardAction::cloneFlyweight() const
{
    return new DrawCardAction(m_quantity, m_targetOwner);
}
