#ifndef BEFORECARDPLAYEDTRIGGER_H
#define BEFORECARDPLAYEDTRIGGER_H

#include "trigger.h"

class CardPlayedTrigger : public Trigger
{
public:
    CardPlayedTrigger(QVector<Action*>* actions,
                            CardType cardType = CardTypes::CARD_MINION,
                            CharacterType characterType = CharacterTypes::MINION,
                            Owner owningPlayer = Owners::ANY_OWNER);
    CardPlayedTrigger(Action* action,
                            CardType cardType = CardTypes::CARD_MINION,
                            CharacterType characterType = CharacterTypes::MINION,
                            Owner owningPlayer = Owners::ANY_OWNER);
    ~CardPlayedTrigger();

private:
    CardType m_cardType;
    CharacterType m_characterType;
    Owner m_owningPlayer;

    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // BEFORECARDPLAYEDTRIGGER_H
