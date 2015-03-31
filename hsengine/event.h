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
    /**
     * @brief The Type enum contains all event types fired by the game engine.
     */
    enum Type {
        /**
         * @brief Represents no event. This will never be fired.
         *
         * @extras
         * - None
         */
        NO_EVENT = 0,

        /**
         * @brief Fired after any card is played.
         *
         * @extras
         * - \c card: [Card] The card played
         *
         * @note Minions can be played from a player's hand but also
         * directly summoned on the battlefield by some actions. These
         * \e summon effects do \b not fire this event.
         */
        AFTER_CARD_PLAYED,

        /**
         * @brief Fired at all turn ends.
         *
         * This event is fired before the control is switched to the other
         * player, and after Abilities::FROZEN conditions are removed.
         *
         * @extras
         * - \c player: [Player] The player whose turn is ended
         */
        AT_TURN_END,

        /**
         * @brief Fired before any card is played.
         *
         * @extras
         * - \c card: [Card] The card played
         *
         * @note Minions can be played from a player's hand but also
         * directly summoned on the battlefield by some actions. These
         * \e summon effects do \b not fire this event.
         */
        BEFORE_CARD_PLAYED,

        /**
         * @brief Fired at each attack action resolution.
         *
         * This event is fired before the actual attack occurs (before the
         * attacker connects to its target).
         *
         * @extras
         * - \c attackingCharacter: [Character] The attacker
         * - \c attackedCharacter: [Character] The \e initial target
         */
        CHARACTER_ATTACKS,

        /**
         * @brief Fired whenever a character is damaged.
         *
         * This event is fired after the damages are done but before any death
         * resolution.
         *
         * @extras
         * - \c damager: [Card] The source of the damages if any, \c NULL
         * otherwise.
         * - \c damagedCharacter: [Character] The damaged character
         */
        CHARACTER_DAMAGED,

        /**
         * @brief Fired whenever a character is healed.
         *
         * This event is fired after the healing is done.
         *
         * @extras
         * - \c healer: [Card] The source of the healing if any, \c NULL
         * otherwise.
         * - \c healedCharacter: [Character] The damaged character
         */
        CHARACTER_HEALED,

        /**
         * @brief Fired whenever a minion dies.
         *
         * This event is fired at the end of the resolution phase, when deaths
         * are resolved one by one. Deaths are resolved in the order of play of
         * the dying minions. This event is fired after the resolution of the
         * dying minion's deathrattles, if any.
         *
         * @extras
         * - \c dyingMinion: [Minion] The dying minion
         */
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
