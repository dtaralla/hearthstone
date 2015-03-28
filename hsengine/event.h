#ifndef EVENT_H
#define EVENT_H

#include <QHash>
#include <QString>
#include <QVariant>
#include "stringable.h"
#include "vptr.h"

typedef const char* EventParam;
namespace EventParams {
    EventParam const CARD_PLAYED              = "card";
    EventParam const DAMAGED_CHARACTER        = "damagedCharacter";
    EventParam const DAMAGER                  = "damager";
    EventParam const DYING_MINION             = "dyingMinion";
    EventParam const PLAYER_ENDING_TURN       = "player";
    EventParam const ATTACKED_CHARACTER       = "attackedCharacter";
    EventParam const ATTACKING_CHARACTER      = "attackingCharacter";
    EventParam const HEALED_CHARACTER         = "healedCharacter";
    EventParam const HEALER                   = "healer";
}

class Event : public Stringable
{
public:
    enum Type {
        NO_EVENT = 0,
        AFTER_CARD_PLAYED,
        AT_TURN_END,
        BEFORE_CARD_PLAYED,
        CHARACTER_ATTACKS,
        CHARACTER_DAMAGED,
        CHARACTER_HEALED,
        MINION_DIES
    };

    Event(Type eventType);
    Event(const Event& e);
    virtual ~Event();

    Type type() const;

    QVariant extra(EventParam const key) const;
    void putExtra(EventParam const key, const QVariant& value);
    template<class T> void putExtra(EventParam const key, const T* value)
    {
        putExtra(key, VPtr<T>::AsQVariant(value));
    }


protected:
    Type m_type;
    QHash<QString, QVariant> m_extras;

    // Stringable interface
public:
    QString toString() const;
};

#endif // EVENT_H
