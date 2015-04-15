/**
 * @defgroup actions Actions
 * @brief These classes are used to represent the various actions that can be
 *        executed on the simulator.
 *
 * Actions are divided into two groups. Some actions are designed to be used in
 * custom card databases, making third-party developers able to create their
 * own cards without requiring from them to dive into the simulator code.
 * However, special actions like ending the turn (EndTurnAction), making a
 * minion attack (AttackAction), playing a card (PlayAction) or summoning a
 * minion (SummonMinionAction) are not to be used by third-party developers, as
 * they depict the core mechanics of the game engine itself.
 *
 * @sa Action
 */
/**
 * @defgroup gameActions Game Actions
 *
 * @brief These classes are used to represent the various actions that can be
 *        executed on the game engine.
 *
 * The particularity of these actions is that none are accessible to card
 * databases, or in other words outside of the game engine. For instance, a
 * spell cannot have an EndTurnAction as a consequence.
 *
 * @ingroup actions
 *
 * @sa Action
 */
/**
 * @defgroup cardActions Card Actions
 *
 * @brief These classes are used to represent the various actions that can be
 *        executed by cards.
 *
 * The particularity of these actions is that they are all accessible to card
 * databases.
 *
 * @ingroup actions
 *
 * @sa Action
 */



#ifndef ACTION_H
#define ACTION_H

#include <QVector>
#include "stringable.h"
#include "game_globals.h"

class Card;
class Event;

/**
 * @author David Taralla
 * @date 2015
 *
 * @brief The Action class is an abstract class specifying the interface any
 *        action has to implement, whatever its type.
 *
 * An Action is an object precisely describing something that can be executed to
 * alter the game state. Some examples:
 *   \li Deal 1 damage to an enemy minion
 *   \li End the turn
 *   \li Heal 2 HP to all minions
 *   \li ...
 *
 * This description is implemented by child classes of Action. Along with this
 * description comes an implementation of the Action, materialized by the
 * resolve() factory method. This method will alter the game state according to
 * the nature of the action (its class) and its parameters (its constructor
 * arguments, most of the time).
 *
 * Software-engineering-speaking, the Action class (and all its subclasses) is
 * simultaneously a flyweight instance factory and a flyweight instance.
 *
 * Because I opted to represent a card in the Hearthstone Simulator as an
 * intrinsic state (a flyweight: the description of the card) coupled with an
 * extrinsic state (the current HP of the card, the enchantments it benefits
 * from,...), an Action can be one of two things: either a description, or a
 * description AND its implementation. An Action linked to a CardIdentity flyweight
 * is itself a flyweight. When a Card is instanciated from a CardIdentity
 * flyweight, the actions linked to the CardIdentity are also instanciated for this
 * very Card, giving it the context it needs to execute itself. We call this
 * card the <em>source card</em> of the Action.
 *
 * An Action which has no source card usually cannot compute its resolution, as
 * it is missing context to execute itself. Take the following example.
 *
 * Let a spell card be <em>Fireball: Deal 1 damage to an enemy minion</em>. The
 * SpellInfo flyweight representing the Spell card has a DealDmgHealingAction,
 * with a parameter \c 1 for the amount of damage to deal and another parameter
 * describing the fact that the target has to be an enemy minion. There, the
 * Action is nothing but a description of the action linked to the spell;
 * indeed, the Action could not be executed as it is not linked to some Card.
 * It thus has no way for now to depict a player as an enemy or a friend, and
 * thus can not evaluate its possible targets. When instanciating the card
 * flyweight and putting the resulting Card in a Player's hand, the Action will
 * be linked to a real card, which has an owner, instead of the description of
 * a card. From this information, the Action will be able to evaluate what its
 * possible targets are as it now has a reference to what it considers an ally.
 *
 * Instanciation of an Action flyweight or linking an Action to a Card is the
 * same thing in Hearthstone Simulator: you use the setSourceCard() method.
 * This method instanciate an Action linked to the given Card, leaving the
 * original flyweight untouched. One can therefore see any Action subclass as a
 * factory of actions of the same type, created with the same parameters, but
 * linked to a particular Card.
 *
 * Actions are usually linked to one and only one Card, named the <em>source
 * card</em>, even @ref gameActions. Indeed, some @ref gameActions like
 * AttackAction, PlayAction or SummonMinionAction also require a source Card.
 * On the other hand, some @ref gameActions like EndTurnAction are not linked
 * to any card. Attaching one to them will result in an undefined behaviour.
 *
 * @ingroup actions
 *
 * @sa actions, gameActions, cardActions
 */
class Action : public Stringable
{
public:
    /**
     * @brief Creates an Action flyweight.
     *
     * Any Action created this way cannot execute using resolve(). You must
     * first instanciate this flyweight using its setSourceCard() method, and
     * use the resulting object to execute the Action described by this
     * flyweight.
     *
     * @note Some subclasses like SummonMinionAction or PlayAction enforce to
     * be linked to a Card at construction. These are therefore automatically
     * considered as instances.
     *
     * @note Some sublcasses like EndTurnAction or DestroyEnchantmentAction do
     * not require to be linked to a Card. Instanciating these could end up in
     * undefined behaviour.
     */
    Action();

    /**
     * @brief Destroys an Action.
     */
    virtual ~Action();

    /**
     * @brief Get the Card linked to this Action.
     *
     * Call this only on an instanciated Action.
     *
     * @return The Card linked to this Action if it is an instanciation of an
     *         Action flyweight, NULL if this is a flyweight.
     */
    virtual Card* source() const;

    /**
     * @brief type Get the type of this Action.
     * @return The type of this Action.
     */
    virtual ActionType type() const = 0;

    /**
     * @brief Asks this Action to resolve itself.
     *
     * Executes the action by modifying the game state accordingly.
     *
     * Calling this method can result in a chained series of events, triggering
     * other Actions.
     *
     * @warning One should <b>always</b> call resolvable() first, and ensure it
     * returns \c true before calling this method. Failing to do so might end
     * up in undefined behaviour.
     * @warning If this is a flyweight, calling this method will have an
     * undefined behaviour.
     *
     * @param e The Event triggering this Action, if any.
     *
     * @sa resolvable()
     */
    virtual void resolve(const Event* e = NULL) = 0;

    /**
     * @brief Tells whether this Action deletes itself
     *        automatically after it finished its resolution.
     * @return \c true if deletes itself, \c false otherwise.
     */
    virtual bool deletesItselfAfterResolve() const;

    /**
     * @brief Tells whether this Action is resolvable regarding the
     *        current state of the game.
     *
     * An Event \a e can cause an Action to trigger. However, this Action might
     * not be resolvable at the time the Event is fired. Hence, one can use
     * this method to determine whether or not this Action should be executed.
     *
     * Refer to each subclass documentation for a detailed description of this
     * method.
     *
     * @warning Unless absolutely sure, this method should <b>always</b> be
     * called before calling resolve(). Failing to do so might end up in
     * undefined behaviour.
     *
     * @note Targeted actions are resolvable if and only if there exists at
     * least 1 target satisfying the targeting conditions on the board.
     * Random-target and area-of-effect actions are always resolvable.
     *
     * @param e The Event triggering this Action, if any.
     * @return \c true if resolvable, \c false otherwise.
     *
     * @sa resolve()
     */
    virtual bool resolvable(const Event* e = NULL) const;

    /**
     * @brief Instanciates this Action and link the created
     *        instance to Card \a c.
     *
     * Instances created this way may be executed safely using resolve(). Any
     * Action constructed in another way cannot be executed, unless explicitely
     * stated.
     *
     * @param c The Card that will be linked to the created instance.
     * @return A perfect clone of this Action, but linked to Card \a c.
     */
    virtual Action* setSourceCard(Card* c) const;

    /**
     * @brief Tells whether this Action needs input from the
     * owner player to select a target during its resolution.
     *
     * @return \c true if targeted and selection required, \c false otherwise.
     */
    virtual bool isTargetedAction() const;

    /**
     * @internal
     * @brief Increase the damage dealt by this action, if this is a
     * damaging action.
     *
     * Used by damaging actions that would be enchanted.
     *
     * @param quantity The quantity
     *
     * @deprecated Placeholder for later... Do not use for now.
     *
     * @sa DamagingSpellAction
     */
    virtual void addSpellDmg(int quantity);

protected:
    /**
     * @brief Create a copy of this Action's flyweight.
     *
     * The returned Action source card is set to NULL, as the returned object is
     * a flyweight.
     *
     * @return A clone which is a perfect copy of this Action's flyweight.
     */
    virtual Action* cloneFlyweight() const = 0;

    /**
     * @brief The Card this Action is linked to.
     *
     * If this Action is a flyweight, this contains NULL.
     */
    Card* m_card;
};

#endif // ACTION_H
