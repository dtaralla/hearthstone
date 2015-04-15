#ifndef CHARACTER_H
#define CHARACTER_H

#include "card.h"
#include "characteridentity.h"

class Enchantment;

/**
 * @brief The Character class represents a character a player interacts with.
 *
 * The Character class is abstract because a Character has to be a Minion or a
 * Hero. Indeed, according to its real type, a Character will have different
 * behavior when it is played: heroes can't be played while minions can.
 * Despite some differences, most of the minions and heroes behavior is the
 * same, hence the presence of this intermediate abstract class in the cards
 * hierarchy. Finally, the game card themselves often refer to \e characters
 * even though they sometime specifically talk about minions or heroes.
 *
 * For more information about the hierarchy of card types, how cards are
 * represented and how a Card is different from a CardIdentity, see Card.
 *
 * @sa Card, Minion, Hero
 * @ingroup hsengine
 */
class Character : public Card
{
public:
    /**
     * @brief Constructor.
     *
     * @param baseChar The identity of this character
     * @param owner The player owning this card. Might be \c NULL, as long as a
     * non-\c NULL owner is given when calling initCard().
     *
     * @note A Character has \e always to be initialized using its initCard()
     * method before being usable.
     */
    Character(CharacterIdentity const* baseChar, Player* owner = NULL);

    /**
     * @brief Destructor.
     */
    virtual ~Character();

    /**
     * @brief Gets the identity of this character, which is constant accross
     * games and whose value is completely defined by the loaded database file.
     *
     * Instead of duplicating the base information of all cards sharing the
     * same identity, each card has a \e base CardIdentity common to all cards
     * having the same identity. The information about the card itself (its
     * description, independent of the current game) is contained in another
     * object, a CardIdentity object, wich can be used to compare this very \e
     * instance characteristics to the original ones.
     *
     * @sa CharacterIdentity
     *
     * @return A CharacterIdentity object representing the information shared
     * by all characters having the same identity than this one.
     */
    virtual const CardIdentity* base() const;

    /**
     * @return CardTypes::CARD_CHARACTER
     */
    virtual CardType type() const;

    /**
     * @brief Gets this character subtype.
     *
     * See CharacterIdentity::characterType().
     *
     * @sa CharacterTypes, CharacterIdentity::characterType()
     *
     * @return This character subtype.
     */
    virtual CharacterType characterType() const;

    /**
     * @brief Gets the attack action of this character.
     *
     * This is an AttackAction object whose source card is set to this
     * character.
     *
     * @return The attack action of this character.
     */
    virtual Action* attackAction() const;

    /**
     * @brief Tells whether the "has attacked" flag of this character is set or
     * not.
     *
     * This flag is managed by the Game and the attackAction() resolution code.
     * This class never sets this flag itself.
     *
     * @return Whether the "has attacked" flag of this character is set or not.
     *
     * @sa setHasAttacked()
     */
    virtual bool hasAttacked() const;

    /**
     * @brief Sets the "has attacked" flag of this character.
     *
     * This flag is managed by the Game and the attackAction() resolution code.
     * This class never sets this flag itself.
     *
     * @param hasAttacked the new value of the flag.
     *
     * @sa hasAttacked()
     */
    virtual void setHasAttacked(bool hasAttacked);

    /**
     * @brief Gets the maximum health of this character, all applied
     * enchantments included.
     *
     * This is never smaller than hp().
     *
     * @return The maximum health of this character. This can never be
     * negative.
     */
    virtual int maxHp() const;

    /**
     * @brief Gets the current health of this character.
     *
     * This can never be greater than maxHp().
     *
     * @return The current health of this character. This can be negative.
     */
    virtual int hp() const;

    /**
     * @brief Gets the attack of this character, all applied enchantments
     * included.
     *
     * @return The attack of this character. This can never be negative.
     */
    virtual int atk() const;

    /**
     * @brief Tells whether this character is dying or not.
     *
     * This flag is set with setDying() when the character is scheduled for
     * death resolution. Once a character has been set \e dying, there is no
     * coming back for it in the game.
     *
     * A dying character does not necessarily have its hp() <= 0 (for instance
     * if it is simply killed by a spell).
     *
     * @return Whether this character is dying or not.
     *
     * @sa setDying()
     */
    virtual bool dying() const;

    /**
     * @brief Gets the quantity of trigger powers this character has, all
     * applied enchantments included.
     *
     * See Card::triggerPowersCount() for more information.
     *
     * @return The quantity of trigger powers this character has. This can
     * never be a negative value.
     */
    virtual int triggerPowersCount() const;

    /**
     * @brief Damage this character.
     *
     * This is the common way to reduce the current hp() of this character.
     *
     * @param dmgSource The source of this damage. Can be \c NULL is the source
     * is a rule of the game (for instance, Fatigue cards).
     *
     * @param dmg The amount of damage to deal. Must be >= 0.
     *
     * @triggers
     * If \a dmg > 0, Event::CHARACTER_DAMAGED with
     *      - \c damager : \a dmgSource (can be \c NULL if the source is a
     *        Fatigue card).
     *      - \c damagedCharacter : this character.
     */
    virtual void dealDmg(Card* dmgSource, int dmg);

    /**
     * @brief Heal this character.
     *
     * This is the common way to increase the current hp() of this character.
     * If hp() + \a healing > maxHp(), hp() will be increased by an amount of
     * maxHp() - hp() only (this ensures that hp() is never greater than
     * maxHp()).
     *
     * @param healingSource The source of this healing.
     *
     * @param healing The amount of healing to do. Must be >= 0.
     *
     * @triggers
     * If \a healing > 0, Event::CHARACTER_HEALED with
     *      - \c healer : \a healingSource.
     *      - \c healedCharacter : this character.
     */
    virtual void heal(Card* healingSource, int healing);

    /**
     * @brief Modifies directly the hp() of this character, without firing any
     * event.
     *
     * Even using this method, hp() can never go above maxHp().
     *
     * @param modification The modification on hp() to apply. May be postive or
     * negative.
     */
    virtual void modifyHp(int modification);

    /**
     * @brief Modifies the maximum health of this character.
     *
     * If this would make maximum health go below 1, this kills this character.
     *
     * When maximum health is increased, the character's current health
     * increases by the same amount.
     *
     * When maximum health is decreased, the actual health of the character
     * will only decrease as far as the new maximum.
     *
     * Even if hp() is decreased (resp. increased), this is \b not considered
     * as damage (resp. healing) and thus this does not trigger
     * Event::CHARACTER_DAMAGED (resp. Event::CHARACTER_HEALED).
     *
     * @param modification The modification on maxHp() to apply.
     */
    virtual void addMaxHPModifier(int modification);

    /**
     * @brief Modifies the attack of this character.
     *
     * When Attack is decreased, the character's Attack will only decrease as
     * far as 0. Characters with 0 attack cannot attack.
     *
     * @param modification The modification on atk() to apply.
     */
    virtual void addAtkModifier(int modification);

    /**
     * @brief Adds abilities to this character.
     *
     * @param a The Ability bitstring to OR with the current Ability bitstring
     * of this character to add.
     */
    virtual void addAbilities(Ability a);

    /**
     * @brief Sets the "is dying" flag of this character.
     *
     * @param dying The new value of this flag.
     *
     * @sa dying()
     */
    virtual void setDying(bool dying);

    /**
     * @brief Tells whether this character is injured.
     *
     * Shortcut for <code>hp() < maxHp()</code>
     *
     * @return Whether this character is injured or not.
     */
    virtual bool injured() const;

    /**
     * @brief Tells whether this character has some given abilities.
     *
     * @param a An Ability bitstring containing the abilities to test.
     *
     * @return \c true if all abilities expressed in \a a are possessed by this
     * character, \c false otherwise.
     */
    virtual bool hasAbility(Ability a) const;

    QVector<Enchantment*>* takeEnchantmentsBefore(const Enchantment* s);
    virtual void removeEnchantment(Enchantment* e);
    virtual void clearEnchantmentEffects();

    /**
     * @brief Sets the "is silenced" flag of this character.
     *
     * A character should have this flag set if it was at least once the target
     * of a SilenceTargetAction.
     *
     * The only possible way of reseting this flag is to return this character
     * in the hand of his owner.
     *
     * Silence is an ability which removes all current card text, enchantments,
     * and abilities from the targeted character. It does not remove damage or
     * minion type.
     *
     * @param silenced The new value of the "is silenced" flag.
     *
     * @sa isSilenced()
     */
    virtual void setSilenced(bool silenced);

    /**
     * @brief Gets the value of the "is silenced" flag of this character.
     *
     * A character should have this flag set if it was at least once the target
     * of a SilenceTargetAction.
     *
     * The only possible way of reseting this flag is to return this character
     * in the hand of his owner.
     *
     * Silence is an ability which removes all current card text, enchantments,
     * and abilities from the targeted character. It does not remove damage or
     * minion type.
     *
     * @return The value of the "is silenced" flag of this character.
     *
     * @sa setSilenced();
     */
    virtual bool isSilenced() const;

    virtual bool listensTo(const Event &e) const;

    /**
     * @brief Freezes this character.
     *
     * Freeze is a condition that can affect minions or heroes. Frozen
     * characters miss their next possible attack. Frozen heroes can still use
     * their hero power while being frozen, and can use cards and command
     * minions as usual. Frozen characters will still defend themselves if
     * attacked. Silencing a frozen character will remove the freeze effect.
     *
     * Practically, this enchants this character with an enchantment that
     * provides the Abilities::FROZEN ability. This enchantment can be removed
     * either by silencing this character or by calling unfreeze().
     *
     * @param sourceCard The card being the source of this freezing. Cannot be
     * \c NULL.
     */
    virtual void freeze(Card* sourceCard);

    /**
     * @brief Removes the Frozen condition on this character.
     *
     * Practically, this removes the frozen enchantment set with freeze().
     *
     * This takes care of deleting the Frozen effect (as it was this class
     * which generated it in the first place).
     *
     * @warning This character \e has to be frozen in the first place. Meaning,
     * <code>this->hasAbility(Abilities::FROZEN)</code> returns \c true.
     */
    virtual void unfreeze();

protected:
    /**
     * @brief The identity of this card.
     */
    CharacterIdentity const* m_base;

    /**
     * @brief The additional abilities this character benefits (or suffer)
     * from.
     *
     * Applying enchantments can modify this value in the case where these
     * enchantment have ability modifiers.
     */
    Ability m_additionalAbilities;

    /**
     * @brief The AttackAction of this character.
     * @sa attackAction()
     */
    Action* m_attackAction;

    /**
     * @brief The max health modifier this character benefits (or suffer) from.
     *
     * Applying enchantments can modify this value in the case where these
     * enchantment have max health modifiers.
     */
    int m_maxHpModifier;

    /**
     * @brief The current health of this character.
     *
     * This is never greater than <code>m_base->hp() + m_maxHpModifier</code>.
     *
     * @sa hp()
     */
    int m_curHp;

    /**
     * @brief The attack modifier this character benefits (or suffer) from.
     *
     * Applying enchantments can modify this value in the case where these
     * enchantment have attack modifiers.
     */
    int m_atkModifier;

    /**
     * @brief The "has attacked" flag.
     * @sa setHasAttacked(), hasAttacked()
     */
    bool m_hasAttacked;

    /**
     * @brief The "is dying" flag.
     * @sa setDying(), dying()
     */
    bool m_dying;

    /**
     * @brief The "is silenced" flag.
     * @sa setSilenced(), isSilenced()
     */
    bool m_silenced;

    /**
     * @brief The Frozen enchantment this character suffers from.
     *
     * \li If <code>this->hasAbility(Abilities::FROZEN)</code> returns \c true,
     * this is the Frozen enchantment this character suffers from.
     * \li If <code>this->hasAbility(Abilities::FROZEN)</code> returns
     * \c false, this is equal to \c NULL.
     */
    Enchantment* m_frozenEnchantment;
};

#endif // CHARACTER_H
