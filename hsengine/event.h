/**
 * @file event.h
 *
 * @brief Defines the various event types supported by the Hearthstone
 * Simulator.
 *
 * @ingroup hsengine
 */

#ifndef EVENT_H
#define EVENT_H

#include <QHash>
#include <QString>
#include <QVariant>
#include "stringable.h"
#include "vptr.h"

/**
 * @brief Any variable of this type represents an event response type.
 *
 * <i>%Event responses</i> are kind of like event parameters set when the even
 * is fired. These parameters are information about what caused the event and
 * the context in which it was fired.
 *
 * The EventParams namespace contains all supported event responses (ie., all
 * values an \c EventParam variable can take).
 *
 * @sa EventParams
 * @ingroup hsengine
 */
typedef const char* EventParam;

/**
 * @brief Aggregates all possible values for variables of type EventParam.
 */
namespace EventParams {
    /**
     * @brief The card played. This parameter is available only in response to
     * a Event::AFTER_CARD_PLAYED or Event::BEFORE_CARD_PLAYED event.
     */
    EventParam const CARD_PLAYED              = "card";

    /**
     * @brief The damaged character. This parameter is available only in
     * response to a Event::CHARACTER_DAMAGED event.
     */
    EventParam const DAMAGED_CHARACTER        = "damagedCharacter";

    /**
     * @brief The character being the damage source. This parameter is
     * available only in response to a Event::CHARACTER_DAMAGED event.
     */
    EventParam const DAMAGER                  = "damager";

    /**
     * @brief The dying minion. This parameter is available only in response to
     * a Event::MINION_DIES event.
     */
    EventParam const DYING_MINION             = "dyingMinion";

    /**
     * @brief The player ending the turn. This parameter is available only in
     * response to a Event::AT_TURN_END event.
     */
    EventParam const PLAYER_ENDING_TURN       = "player";

    /**
     * @brief The attacked character. This parameter is available only in
     * response to a Event::CHARACTER_ATTACKS event.
     */
    EventParam const ATTACKED_CHARACTER       = "attackedCharacter";

    /**
     * @brief The attacking character. This parameter is available only in
     * response to a Event::CHARACTER_ATTACKS event.
     */
    EventParam const ATTACKING_CHARACTER      = "attackingCharacter";

    /**
     * @brief The healed character. This parameter is available only in
     * response to a Event::CHARACTER_HEALED event.
     */
    EventParam const HEALED_CHARACTER         = "healedCharacter";

    /**
     * @brief The character being the healing source. This parameter is
     * available only in response to a Event::CHARACTER_HEALED event.
     */
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
