/**
 * @file game_globals.h
 *
 * @brief Defines the types and namespaces Hearthstone Simulator uses.
 *
 * This file defines various types and namespaces containing global constants
 * used by the Hearthstone Simulator. Those namespaces might also contain
 * utility functions.
 */

#ifndef GAME_GLOBALS
#define GAME_GLOBALS

#include <QVector>
#include <QMap>

class Player;
class Minion;


/**
 * @brief A variables of this type represents a qualifier decsribing the
 * relative owner of something in the game.
 *
 * A Card has an owner which is a player. The actions that take place when
 * playing it often are stated as "Do something to some @e enemy guy". Thus,
 * actions are usually defined relatively to the owner of the card it is linked
 * to. This type represents this relative information.
 *
 * @note This type is implemented as a bitstring of values from the Owners
 * namespace. One can combine its values using bitwise operators.
 *
 * @sa Owners
 */
typedef uint Owner;

/**
 * @brief A variables of this type represents a character type id.
 *
 * Some actions in Hearthstone only apply to a given type of characters, for
 * instance
 * @li Any minion
 * @li Any beast
 * @li Any character
 * @li ...
 * This type is used for such situations.
 *
 * @note This type is implemented as a bitstring of values from the
 * CharacterTypes namespace. One can combine its values using bitwise operators.
 *
 * @sa CharacterTypes
 */
typedef uint CharacterType;

/**
 * @brief A variables of this type represents a card type id.
 *
 * In Hearthstone, cards can represent spells, minions, heroes, secrets,... This
 * type represents this information.
 *
 * @note This type is implemented as a bitstring of values from the CardTypes
 * namespace. One can combine its values using bitwise operators.
 *
 * @sa CardTypes
 */
typedef uint CardType;

/**
 * @brief A variables of this type represents a set of ability ids.
 *
 * In Hearthstone, there various built-in, generic abilities which are part of
 * the games general rules. These are for example
 * @li Taunt
 * @li Charge
 * @li Frozen
 * @li Divine Shield
 * @li ...
 *
 * @note Characters can have multiple abilities simultaneously. This type is
 * thus implemented as a bitstring of values from the Abilities namespace.
 * One can combine its values using bitwise operators to represent the
 * abilities a character has.
 *
 * @warning Even though the type name is Ability, a variable of this type
 * represents a @b set of abilities! Do not use == to see if the set contains
 * the ability, but use the @c & bitwise operator.
 *
 * @sa Abilities
 */
typedef uint Ability;

/**
 * @brief Aggregates possible individual values for the Owner type and some
 * utility functions on this type.
 *
 * @sa Owner
 */
namespace Owners {
    const Owner ALLY  = 1 << 0; //!< Represents the ally ownership, relative to a given context.
    const Owner ENEMY = 1 << 1; //!< Represents the enemy ownership, relative to a given context.
    const Owner ANY_OWNER = ALLY | ENEMY; //!< Represents any ownership.

    /**
     * @brief Gets a list of the possible individual values the Owner type can
     * take.
     *
     * Mainly used to parse card databases.
     *
     * @return A list of the possible individual values for the Owner type.
     */
    QVector<Owner> availableOwners();

    /**
     * @brief Gets a string representation of an Owner-typed value.
     *
     * @param o The value to translate; should not be a combination unless this
     * combination is registered in the Owners namespace.
     *
     * @return A string representation of \a o.
     */
    QString toString(Owner o);
}

/**
 * @brief Aggregates possible individual values for the CharacterType type and
 * some utility functions on this type.
 *
 * @sa CharacterType
 */
namespace CharacterTypes {
    const CharacterType UNDEAD    = 1 << 0; //!< Represents an undead character.
    const CharacterType BEAST     = 1 << 1; //!< Represents a beast.
    const CharacterType DEMON     = 1 << 2; //!< Represents a demon.
    const CharacterType DRAGON    = 1 << 3; //!< Represents a dragon.
    const CharacterType GENERAL   = 1 << 4; //!< Represents a common character.
    const CharacterType MECH      = 1 << 5; //!< Represents a mech.
    const CharacterType MURLOC    = 1 << 6; //!< Represents a murloc.
    const CharacterType PIRATE    = 1 << 7; //!< Represents a pirate.
    const CharacterType TOTEM     = 1 << 8; //!< Represents a totem.
    const CharacterType MINION    = UNDEAD|BEAST|DEMON|DRAGON|GENERAL|MECH|MURLOC|PIRATE|TOTEM; //!< Represents any minion.
    const CharacterType HERO      = 1 << 9; //!< Represents a hero.
    const CharacterType CHARACTER = MINION | HERO; //!< Represents any character.

    /**
     * @brief Gets a list of the possible individual values the CharacterType
     * type can take.
     *
     * Mainly used to parse card databases.
     *
     * @return A list of the possible individual values for the CharacterType
     * type.
     */
    QVector<CharacterType> availableTypes();

    /**
     * @brief Gets a string representation of an CharacterType-typed value.
     *
     * @param ct The value to translate; should not be a combination unless this
     * combination is registered in the CharacterTypes namespace.
     *
     * @return A string representation of \a ct.
     */
    QString toString(CharacterType ct);
}

/**
 * @brief Aggregates possible individual values for the CardType type and some
 * utility functions on this type.
 *
 * @sa CardType
 */
namespace CardTypes {
    const CardType CARD_MINION    = 1 << 0; //!< Represents a minion card
    const CardType CARD_HERO      = 1 << 1; //!< Represents a hero card
    const CardType CARD_CHARACTER = CARD_MINION | CARD_HERO;  //!< Represents a character card
    const CardType CARD_SPELL     = 1 << 2; //!< Represents a spell card
    const CardType CARD_SECRET    = 1 << 3; //!< Represents a secret card
    const CardType CARD_WEAPON    = 1 << 4; //!< Represents a weapon card

    /**
     * @brief Gets a list of the possible individual values the CardType type
     * can take.
     *
     * Mainly used to parse card databases.
     *
     * @return A list of the possible individual values for the CardType type.
     */
    QVector<CardType> availableTypes();

    /**
     * @brief Gets a string representation of an CardType-typed value.
     *
     * @param ct The value to translate; should not be a combination unless this
     * combination is registered in the CardTypes namespace.
     *
     * @return A string representation of \a ct.
     */
    QString toString(CardType ct);
}

/**
 * @brief Aggregates possible individual values for the Ability type and some
 * utility functions on this type.
 *
 * @sa Ability
 */
namespace Abilities {
    const Ability NO_ABILITY = 0; //!< Represents an empty ability set.

    /**
     * @brief Represents the Charge ability.
     *
     * Charge is an ability allowing a minion to attack the same turn it is
     * summoned.
     *
     * @sa <a href="http://hearthstone.gamepedia.com/Charge_(ability)">
     * Article on Hearthstone Wiki</a>
     */
    const Ability CHARGE     = 1 << 0;

    /**
     * @brief Represents the Taunt ability.
     *
     * Taunt is a minion ability which forces the opposing player to direct any
     * melee attacks toward enemy targets with this ability. Taunts serve to
     * protect their allies by forcing the enemy to deal with them first,
     * preventing them from attacking other friendly minions or the controlling
     * hero until the Taunts are removed. However, Taunts have no effect on
     * spells, hero powers or other direct damage effects. If the opponent has
     * one or more Taunt minions on the battlefield, the player will only be
     * able to direct melee attacks at one of the minions with Taunt. However,
     * the opponent is able to choose which enemy Taunt to attack, or may
     * simply choose not to attack.
     *
     * @sa <a href="http://hearthstone.gamepedia.com/Taunt">Article on
     * Hearthstone Wiki</a>
     */
    const Ability TAUNT      = 1 << 1;

    /**
     * @brief Represents the Frozen condition.
     *
     * Frozen is a condition that can affect minions or heroes. Frozen
     * characters miss their next possible attack. Frozen heroes can still use
     * their hero power while being frozen, and can use cards and command
     * minions as usual. Frozen characters will still defend themselves if
     * attacked. Silencing a frozen character will remove the freeze effect.
     *
     * @b Duration: If the character is frozen and it has not attacked this
     * turn, unfreeze it.
     *
     * @sa <a href="http://hearthstone.gamepedia.com/Freeze">Article on
     * Hearthstone Wiki</a>
     */
    const Ability FROZEN     = 1 << 2;

    /**
     * @brief Gets a list of the possible individual values the Ability type
     * can take.
     *
     * Mainly used to parse card databases. Conditions like FROZEN are not
     * included in the list.
     *
     * @return A list of the possible individual values for the Ability type.
     */
    QVector<Ability> availableAbilities();

    /**
     * @brief Gets a string representation of an Ability-typed value.
     *
     * @param a The value to translate; should not be a combination unless this
     * combination is registered in the CardTypes namespace.
     *
     * @return A string representation of \a a.
     */
    QString toString(Ability a);
}

/**
 * @brief Aggregates possible values for the ActionType type and some utility
 * functions on this type.
 *
 * @sa ActionType
 */
namespace ActionTypes {
    /**
     * @brief The ActionType enum contains all possible values for the
     * ActionType type.
     *
     * Practically every value listed in here are available to third-party
     * developers who would create new cards without touching the source code.
     * Some values however are reserved for the game engine, for instance
     * PLAY_CARD, END_TURN, DESTROY_ENCHANTMENT and SUMMON. These cannot be
     * used in the card databases, and thus are not returned by the
     * availableTypes() command.
     *
     * @sa Actions, GameActions, CardActions, ActionType, availableTypes()
     */
    enum ActionType {
        ADD_ARMOR = 0,
        ADD_MANA,
        ATTACK,
        DEAL_DMG,
        DESTROY_ENCHANTMENT,
        DRAW_CARD,
        ENCHANT_CHARACTER,
        END_TURN,
        FREEZE_AFTER_DMG,
        FREEZE_AFTER_HEAL,
        HEAL,
        PLAY_CARD,
        SET_FROZEN,
        SILENCE_TARGET,
        SPECIAL_POWER,
        SUMMON,
        SUMMON_MINIONS
    };

    /**
     * @brief Gets a list of the ActionTypes usable for parsing card databases.
     *
     * Mainly used to parse card databases. GameActions like PLAY_CARD,
     * END_TURN, DESTROY_ENCHANTMENT and SUMMON are not listed.
     *
     * @return A list of the ActionTypes usable for parsing card databases.
     *
     * @sa GameActions
     */
    QVector<ActionType> availableTypes();

    /**
     * @brief Gets a string representation of an ActionType value.
     *
     * @param a The value to translate.
     *
     * @return A string representation of \a a.
     */
    QString toString(ActionType a);
}

/**
 * @brief A variable of this type represents a built-in action type id.
 *
 * This simulator is built in such a way that it is fairly easy for a
 * third-party developer to create new cards based on the existing possible
 * game actions. This is done by providing him/her with built-in actions that
 * can be parametrized as he/she wishes.
 *
 * Let two actions be "Draw one card" and "Make the opponent draw a
 * card". The core action, "Draw card", is the common point of the two actions.
 * The core action is called the ActionType. Thus, here, both actions have the
 * same ActionType, even though they have different behavior.
 */
typedef enum ActionTypes::ActionType ActionType;

#endif // GAME_GLOBALS

