#include "cardplayedtrigger.h"
#include "card.h"
#include "character.h"
#include "player.h"

CardPlayedTrigger::CardPlayedTrigger(QVector<Action*>* actions,
                                                 CardType cardType,
                                                 CharacterType characterType,
                                                 Owner owningPlayer) :
    Trigger(actions),
    m_cardType(cardType),
    m_characterType(characterType),
    m_owningPlayer(owningPlayer)
{

}

CardPlayedTrigger::CardPlayedTrigger(Action* action,
                                                 CardType cardType,
                                                 CharacterType characterType,
                                                 Owner owningPlayer) :
    Trigger(action),
    m_cardType(cardType),
    m_characterType(characterType),
    m_owningPlayer(owningPlayer)
{

}

CardPlayedTrigger::~CardPlayedTrigger()
{

}



bool CardPlayedTrigger::listensTo(const Event& e) const
{
    Card* playedCard = VPtr<Card>::AsPtr(e.extra("card"));

    if (m_owningPlayer == Owners::ALLY && playedCard->owner() != m_card->owner())
        return false;

    if (m_owningPlayer == Owners::ENEMY &&
        playedCard->owner() != m_card->owner()->opponent())
        return false;

    if (m_cardType & CardTypes::CARD_CHARACTER) {
        if (playedCard->type() & CardTypes::CARD_CHARACTER)
            return ((Character*) m_card)->subtype() & m_characterType;
    }

    return m_cardType & playedCard->type();
}

Trigger* CardPlayedTrigger::clone() const
{
    return new CardPlayedTrigger(m_actions, m_cardType, m_characterType, m_owningPlayer);
}
