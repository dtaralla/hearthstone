#ifndef CHARACTERIDENTITY_H
#define CHARACTERIDENTITY_H

#include "cardidentity.h"

class Action;
class Trigger;

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
    CardType type() const;
    int hp() const;
    int atk() const;
    bool hasAbility(Ability a) const;

    const QVector<Action*>* battlecry() const;
    const QVector<Action*>* deathRattles() const;

protected:
    CharacterType m_type;
    int m_hp;
    int m_atk;
    QVector<Action*> m_battlecry;
    QVector<Action*> m_deathrattle;
    Ability m_abilities;
};

#endif // CHARACTERIDENTITY_H
