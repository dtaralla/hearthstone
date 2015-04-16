#ifndef CHARACTERIDENTITY_H
#define CHARACTERIDENTITY_H

#include "cardidentity.h"

class Action;
class Trigger;

/**
 * @brief The CharacterIdentity class represent a character identity.
 *
 * It regroups all the information to define a character. Minion objects use
 * this class directly as their identity, while Hero objects use a subclass of
 * this one, HeroIdentity, which is a subset of this one. Indeed, heroes
 * never have deathrattles nor battlecries for instance.
 *
 * @ingroup hsengine
 * @sa Character, Minion, Hero, HeroIdentity
 */
class CharacterIdentity : public CardIdentity
{
public:
    /**
     * @brief Constructor.
     *
     * @param id The unique numerical identifier of this identity.
     * @param name The characters name that could possibly be displayed in a UI
     * @param desc The characters description that could possibly be displayed
     * in a UI
     * @param manaCost The base mana cost of all characters having this
     * identity
     * @param hp The base maximum health of all characters having this
     * identity
     * @param atk The base attack of all characters having this identity
     * @param battlecry The battlecry of all characters having this identity. A
     * battlecry is a list of actions to execute when the minion is played.
     * Heroes do not have any battlecries.
     * @param deathrattle The deathrattle of all characters having this
     * identity. A deathrattle is a list of actions to execute when the minion
     * dies. Heroes do not have any deathrattles.
     * @param abilities The base abilities of all characters having this
     * identity
     * @param triggerPowers An associative list containing the base trigger
     * powers of all characters having this identity
     * @param characterType The subtype of all characters having this identity.
     * If CharacterTypes::CHARACTER or CharacterTypes::MINION is given, it
     * will default to CharacterTypes::GENERAL.
     * @param collectible Whether characters with this identity can be put in a
     * deck
     */
    CharacterIdentity(int id,
                  const QString& name, const QString& desc,
                  int manaCost, int hp, int atk,
                  const QVector<Action*>& battlecry,
                  const QVector<Action*>& deathrattle,
                  Ability abilities,
                  const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                  CharacterType subtype = CharacterTypes::GENERAL,
                  bool collectible = true);

    /**
     * @brief Destructor.
     */
    ~CharacterIdentity();

    /**
     * @brief Gets the subtype of this character identity.
     *
     * A character can be many things: an undead minion, a totem minion, a
     * hero, a mech minion,...
     *
     * If this character is a minion, <code>subtype() &
     * CharacterTypes::MINION</code> will return \c true. If it is a hero,
     * <code>subtype() & CharacterTypes::HERO</code> will return true.
     *
     * It is guaranteed that this function never returns aggregate subtypes:
     * \li CharacterTypes::MINION
     * \li CharacterTypes::CHARACTER
     *
     * @sa CharacterTypes
     *
     * @return The subtype of this character identity.
     */
    CharacterType subtype() const;

    /**
     * @brief Gets this character identity's type.
     *
     * @return If <code>this->subtype() & CharacterTypes::HERO</code> returns
     * \c true, this returns CardTypes::CARD_HERO. Otherwise, returns
     * CardTypes::CARD_MINION.
     */
    CardType type() const;

    /**
     * @brief Gets the base max health of all characters having this identity
     * @return The base max health of all characters having this identity. This
     * is always positive.
     */
    int hp() const;

    /**
     * @brief Gets the base attack of all characters having this identity
     * @return The base attack of all characters having this identity. This
     * is always positive.
     */
    int atk() const;

    /**
     * @brief Tells whether characters having this identity have some given
     * abilities.
     *
     * @param a An Ability bitstring containing the abilities to test.
     *
     * @return \c true if all abilities expressed in \a a are possessed by this
     * character identity, \c false otherwise.
     */
    bool hasAbility(Ability a) const;

    /**
     * @brief Gets the battlecry of all characters having this identity.
     * @return The battlecry of all characters having this identity. This is
     * never \c NULL but might be empty.
     */
    const QVector<Action*>* battlecry() const;

    /**
     * @brief Gets the deathrattle of all characters having this identity.
     * @return The deathrattle of all characters having this identity. This is
     * never \c NULL but might be empty.
     */
    const QVector<Action*>* deathrattle() const;

protected:
    /**
     * @brief The subtype of all characters having this identity.
     * @sa subtype()
     */
    CharacterType m_type;

    /**
     * @brief The maximum health of all characters having this identity.
     * @sa hp()
     */
    int m_hp;

    /**
     * @brief The attack of all characters having this identity.
     * @sa atk()
     */
    int m_atk;

    /**
     * @brief The battlecry of all characters having this identity.
     * @sa battlecry()
     */
    QVector<Action*> m_battlecry;

    /**
     * @brief The deathrattle of all characters having this identity.
     * @sa deathrattle()
     */
    QVector<Action*> m_deathrattle;

    /**
     * @brief The abilities of all characters having this identity.
     * @sa hasAbility()
     */
    Ability m_abilities;
};

#endif // CHARACTERIDENTITY_H
