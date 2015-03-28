#include "event.h"
#include "minion.h"
#include "spell.h"
#include "player.h"

Event::Event(Type eventType) :
    m_type(eventType)
{
}

Event::Event(const Event& e) :
    m_type(e.m_type),
    m_extras(e.m_extras)
{

}

Event::~Event()
{

}

Event::Type Event::type() const
{
    return m_type;
}

QVariant Event::extra(const EventParam key) const
{
    if (!m_extras.contains(key))
        qCritical() << "This event (" << m_type << ") has nothing for key" << key;

    return m_extras.value(key);
}

void Event::putExtra(const EventParam key, const QVariant& value)
{
    m_extras.insert(key, value);
}


QString Event::toString() const
{
    switch (m_type) {
        case NO_EVENT:
            qCritical() << "No event triggered!";
            return "No event triggered";

        case AFTER_CARD_PLAYED:
            return "A card (" + VPtr<Card>::AsPtr(extra("card"))->toString() +
                    ") has been played";

        case AT_TURN_END:
            return VPtr<Player>::AsPtr(extra("player"))->name() +
                    " ends its turn";

        case CHARACTER_ATTACKS:
            return VPtr<Character>::AsPtr(extra("attackingCharacter"))->toString() +
                    " attacks " +
                    VPtr<Character>::AsPtr(extra("attackedCharacter"))->toString();

        case CHARACTER_DAMAGED: {
            Card* damager = VPtr<Card>::AsPtr(extra("damager"));
            if (damager == NULL)
                return VPtr<Character>::AsPtr(extra("damagedCharacter"))->toString() +
                    " was damaged";

            return VPtr<Character>::AsPtr(extra("damagedCharacter"))->toString() +
                    " was damaged by " + damager->toString();
        }

        case CHARACTER_HEALED:
            return VPtr<Character>::AsPtr(extra("healedCharacter"))->toString() +
                    " was healed by " +
                    VPtr<Card>::AsPtr(extra("healer"))->toString();

        case BEFORE_CARD_PLAYED:
            return "A card (" + VPtr<Card>::AsPtr(extra("card"))->toString() +
                    ") is going to be played";

        case MINION_DIES:
            return "A minion (" + VPtr<Minion>::AsPtr(extra("dyingMinion"))->toString() +
                    ") died";

        default:
            qCritical() << "This event type should not be used";
            return "This event type should not be used";
    }
}
