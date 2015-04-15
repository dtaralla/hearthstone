#ifndef CARD_H
#define CARD_H

#include <QtDebug>
#include "event.h"
#include "game_globals.h"
#include "stringable.h"

class Player;
class CardIdentity;
class Action;
class Trigger;
class PlayAction;
class Enchantment;

/**
 * @brief The Card class represents a card a player interacts with.
 *
 * There are multiple card types in Hearthstone: minions, spells, secrets,
 * weapons, but also heroes. A Card is what a player plays with.
 *
 * Contrarily to Card objects, CardIdentity objects (or <i>card identities</i>)
 * are abstract description of cards. They describe what cards \e are but also
 * what cards \e do. However, they usually do so by using \e relative
 * qualifiers:
 *
 * \li Deal X damage to an <b>enemy</b> target
 * \li Each time <b>you</b> cast a spell, do something
 * \li Each time <b>this</b> minion takes damage, <b>it</b> gains +1 ATK
 * \li ...
 *
 * It means that what a card's identity describes usually refers to things that
 * can be evaluated only by the very instance which is personifiying this
 * identity. It is the reason why all Card objects need to be initialized
 * before being used: they copy the relative information given by their
 * identity and bind it to themselves. Once this is done, they can evaluate the
 * meaning of qualifiers like \e enemy, \e ally or \e this.
 *
 * To allow one to know which kind of Card it is actually dealing with, the
 * type() factory method is provided. This method is implemented in child
 * classes of Card and returns a CardType. You might wander whether a Card is a
 * Character, regardless of it being a Hero or a Minion. As the CardType is a
 * bitstring, you can check this by using <code>card->type() &
 * CardTypes::CARD_CHARACTER</code>.
 *
 * As we said, there are multiple card types and each has its dedicated
 * characteristics. However, all card types share a set of characteristics:
 * they present a type(), a base(), can provide their owner(), their
 * effectiveManaCost(), which kind of events they listen to,... All cards can
 * also benefit from some enchantments. Because of all these shared
 * characteristics amongst cards, this superclass is the parent of all other
 * card type classes and provides a base implementation for all those common
 * things.
 *
 * @note A Card has \e always to be initialized using its initCard() method
 * before being usable.
 *
 * @sa CardIdentity, CardTypes, CardType, Character, Minion, Hero, Spell
 * @ingroup hsengine
 */
class Card : public Stringable
{
public:
    /**
     * @brief Constructor.
     *
     * @param owner The player owning this card. Might be \c NULL, as long as a
     * non-\c NULL owner is given when calling initCard().
     *
     * @note A Card has \e always to be initialized using its initCard() method
     * before being usable.
     */
    Card(Player* owner = NULL);

    /**
     * @brief Destructor.
     */
    virtual ~Card();

    /**
     * @brief Gets the PlayAction object representing the action of playing
     * this very card.
     *
     * Refer to the documentation of the implementations of this abstract
     * method for more information.
     *
     * @warning Hero cards are the only cards which do \e not have any play
     * actions. All others card types are guaranteed to return a valid
     * PlayAction object.
     *
     * @return A PlayAction object representing the action of playing this very
     * card.
     */
    virtual PlayAction* playAction() = 0;

    /**
     * @brief Gets a list of actions that will be executed as a result of
     * having played this very card.
     *
     * For a Spell, the consequences of playing it would be the list of actions
     * describing its effects; for a Minion, it would be a list of one element
     * containing a SummonMinionAction;...
     *
     * @warning As Hero cards do \e not have any play actions, they also have
     * no play action consequences. All others card types are guaranteed to
     * return a valid list of Action objects.
     *
     * @return A list of actions that will be executed as a result of
     * having played this very card.
     */
    virtual const QVector<Action*>* playActionConsequences() const = 0;

    /**
     * @brief Gets the name of this card, shared by all cards having the same
     * identity.
     *
     * Shortcut to <code>card->base()->name()</code>.
     *
     * @sa CardIdentity::name()
     *
     * @return The name of this card, shared by all cards having the same
     * identity.
     */
    virtual const QString& name() const;

    /**
     * @brief Gets the identity of this card, which is constant accross games
     * and whose value is completely defined by the loaded database file.
     *
     * Instead of duplicating the base information of all cards sharing the
     * same identity, each card has a \e base CardIdentity common to all cards
     * having the same identity. The information about the card itself (its
     * description, independent of the current game) is contained in another
     * object, a CardIdentity object, wich can be used to compare this very \e
     * instance characteristics to the original ones.
     *
     * @sa CardIdentity
     *
     * @return A CardIdentity object representing the information shared by all
     * cards having the same identity than this one.
     */
    virtual CardIdentity const* base() const = 0;

    /**
     * @brief Gets the type of this card.
     *
     * When you only have a <code>Card*</code> pointer, this factory method can
     * come in handy to determine the effective type of the pointed object.
     *
     * If your are not interested in the precise type of this card, you might
     * still be wandering whether a Card is a Character, regardless of it being
     * a Hero or a Minion. As the CardType is a bitstring, you can check this
     * by using a test like <code>card->type() &
     * CardTypes::CARD_CHARACTER</code>.
     *
     * @sa CardIdentity::type(), CardType, CardTypes
     *
     * @return The type of this card.
     */
    virtual CardType type() const = 0;

    /**
     * @brief Initializes the card.
     *
     * This method initializes the card by cloning the necessary information
     * from its base identity that is subject to change along this card
     * lifecycle. If no owner Player was given when constructing this Card,
     * \a cardOwner has to be non-\c NULL.
     *
     * @warning A Card has \e always to be initialized using this method before
     * being usable.
     *
     * @param cardOwner The Player owner of this card. If no owner Player was
     * given when constructing this Card, this field has to be non-\c NULL. If
     * an owner was already given when constructing this Card, it will be
     * replaced by this one.
     */
    virtual void initCard(Player* cardOwner);

    /**
     * @brief Gets the Player owner of this card.
     *
     * Ownership is subject to change in the course of a game if spells taking
     * control of this card are played.
     *
     * @return The Player owning this card.
     */
    virtual Player* owner() const;

    /**
     * @brief Gets the effective mana cost of this card.
     *
     * The effective mana cost of a card is the mana cost given by its
     * identity information, possibly modified by an enchantment, in which case
     * the sum of the base mana and its modifier is returned.
     *
     * @return The effective mana cost of this card. This can never be a
     * negative value.
     */
    virtual int effectiveManaCost() const;

    /**
     * @brief Tells whether this card wants to reacts to an event \a e.
     *
     * This checks whether this card is listening to the type of event \a e.
     * However, conditions may apply on the event, in which case they are also
     * evaluated. For instance, even though a card might want to react to
     * MINION_DIES events, it might only reacts to them if their DYING_MINION
     * argument is an ally minion.
     *
     * For more information about how events are handled, see the Event and
     * Trigger classes.
     *
     * @param e The event that occurred.
     *
     * @return \c true if this card wants to react to \a e, \c false otherwise.
     *
     * @sa Event, Trigger
     */
    virtual bool listensTo(const Event& e) const;

    /**
     * @brief Gets the list of actions to execute in response to event \a e.
     *
     * @warning You should \b never call this method without having first
     * checked that <code>listensTo(e)</code> returned \c true.
     *
     * @param e The event that occurred; should be such that
     * <code>listensTo(e)</code> returns \c true.
     *
     * @return The list of actions to execute in reponse to \a e.
     */
    virtual QVector<Action*>* actionsOnEvent(const Event& e) const;

    /**
     * @brief Gets the quantity of trigger powers this card has.
     *
     * A card has a <i>trigger power</i> if it reacts to some kind of event.
     * A card can have several trigger powers, if it reacts to different kinds
     * of events.
     *
     * Examples of trigger powers are:
     * \li <i>Each time an ally minion dies, draw a card.</i>
     * \li <i>At each turn end, gains +1 ATK</i>
     * \li <i>After a spell is cast, deals 1 damage to all characters.</i>
     *
     * @return The quantity of trigger powers this card has. This can never be
     * a negative value.
     */
    virtual int triggerPowersCount() const;

    /**
     * @brief Adds a mana cost modifier to this card.
     *
     * If \a mod is non-zero, this will change the returned value of
     * effectiveManaCost().
     *
     * @param mod The modifier value.
     */
    virtual void addCostModifier(int mod);

    /**
     * @brief Gives additional trigger powers to this card.
     *
     * The additional trigger powers will be maintained until this card is
     * put into the graveyard or is silenced. If this card is an already
     * silenced character, the trigger powers will still be given as the
     * silence only applies to the enchantments and abilities that were active
     * \e before it was applied.
     *
     * A card has a <i>trigger power</i> if it reacts to some kind of event.
     * A card can have several trigger powers, if it reacts to different kinds
     * of events.
     *
     * Examples of trigger powers are:
     * \li <i>Each time an ally minion dies, draw a card.</i>
     * \li <i>At each turn end, gains +1 ATK</i>
     * \li <i>After a spell is cast, deals 1 damage to all characters.</i>
     *
     * @param tps The trigger powers to give.
     */
    virtual void addTriggerPowers(const QHash<Event::Type, QVector<Trigger*>*>& tps);

    /**
     * @brief Enchants this card.
     *
     * Enchantment this card with \a e, stacking it with previously casted
     * enchantments it would already benefit (or suffer) from.
     *
     * Enchantments are kind of like cards modifiers. They can modify all the
     * charactertistics of a card, from its mana cost to its trigger powers,
     * but also its abilities. See the Enchantment class documentation for
     * more information about them.
     *
     * @param e The Enchantment to apply on this card.
     *
     * @note This calls reApplyEnchantmentEffects() after stacking \a e.
     */
    virtual void addEnchantment(Enchantment* e);

    /**
     * @brief Detach enchantment \a e from this card then calls
     * reApplyEnchantmentEffects().
     *
     * @warning \a e has to be an enchantment attached to this card.
     * @warning This does not \c delete this enchantment object.
     *
     * @param e The enchantment to remove.
     */
    virtual void removeEnchantment(Enchantment* e);

    /**
     * @brief Clears the effects any enchantment would have on this card,
     * leaving it as if it was not benefitting (or suffering) from any of them.
     *
     * @note The enchantments are not detached from this card; only their
     * effects are cleared.
     */
    virtual void clearEnchantmentEffects();

    /**
     * @brief Clears the effects any enchantments would have on this card, then
     * applies the effects of the enchantments present in the enchantment
     * stack.
     *
     * This function is useful when one wants to update the effects of the
     * enchantment stack after it has been altered, for instance because a
     * particular enchantment was removed or because a new enchantment was
     * added.
     */
    virtual void reApplyEnchantmentEffects();

    /**
     * @brief Removes all enchantments that are below \a s in the enchantment
     * stack and returns them.
     *
     * \a s is neither included in the list nor removed from the enchantments
     * stack.
     *
     * @param s The enchantment below which all enchantments will be taken out
     * of the stack.
     *
     * @return A list of all enchantments that are below \a s in the
     * enchantment stack, ordered in the same way they were in the stack.
     */
    virtual QVector<Enchantment*>* takeEnchantmentsBefore(const Enchantment* s);

    /**
     * @brief Gets enchantment stack of this card.
     *
     * Enchantments that were removed because of a Silence are not included in
     * this list; it only contains the enchantments currently attached to this
     * card.
     *
     * @return The list of enchantments currently attached to this card, in the
     * order in which they were attached to it.
     */
    virtual const QVector<Enchantment*>* enchantments() const;

protected:
    /**
     * @brief The owner of this card.
     *
     * Ownership is subject to change in the course of a game if spells taking
     * control of this card are played.
     */
    Player* m_owner;

    /**
     * @brief The mana cost modifier to apply when requesting the effective
     * mana cost of this card.
     */
    int m_costModifier;

    /**
     * @brief The trigger powers associated with this card, which are coming
     * from its identity information.
     */
    QHash<Event::Type, QVector<Trigger*>* > m_triggerPowers;

    /**
     * @brief The trigger powers that were added to this card, independent from
     * its identity.
     *
     * These can be coming from enchantments for instance.
     */
    QHash<Event::Type, QVector<Trigger*>* > m_additionalTriggerPowers;

    /**
     * @brief The enchantment stack.
     *
     * This contains all enchantments attached to this card, ordered by
     * attachment time.
     */
    QVector<Enchantment*> m_enchantments;

    // Stringable interface
public:
    /**
     * @brief Gets a String representation of this card.
     *
     * @return A string using the pattern "%cardName (%cardOwnerName)".
     */
    virtual QString toString() const;
};

#endif // CARD_H
