#ifndef ENCHANTMENT_H
#define ENCHANTMENT_H

#include "stringable.h"
#include "game_globals.h"
#include "event.h"
#include <QHash>

class Trigger;
class Card;

/**
 * @brief The Enchantment class represents any kind of enchantment that can
 * affect any kind of card.
 *
 * Enchantments are kind of like cards modifiers. They can modify all the
 * charactertistics of a card, from its mana cost to its trigger powers,
 * but also its abilities.
 *
 * Enchantments flagged as \e temporary will disappear at the first
 * Event::AT_TURN_END emitted (ie., the next time an EndTurnAction is played).
 *
 * As Action's, an Enchantment object represents both a flyweight factory and
 * a flyweight itself. At the moment a card enchants another one, the
 * Enchantment object it has as reference is specialized using the specialize()
 * method. This clones (deep copy) the reference enchantment and sets its
 * buffedCard() and its sourceCard(), leaving the reference untouched for
 * future reuse in case it could enchant again another card.
 *
 * Note that in the near future, this class will certainly be made abstract and
 * split into several subclasses, one for each type of card to enchant
 * (Character, Weapon, Spell). Because of that, the spell damage modifier is
 * still not hooked completely; please do not try to use it for now.
 *
 * @ingroup hsengine
 */
class Enchantment : public Stringable
{
public:
    /**
     * @brief Constructor.
     *
     *
     *
     * @param temporary Enchantments flagged as \e temporary will disappear at
     * the first Event::AT_TURN_END emitted (ie., the next time an
     * EndTurnAction is played).
     * @param name The enchantment name that could possibly be displayed in a
     * UI
     * @param desc The enchantment description that could possibly be displayed
     * in a UI
     * @param abilities This bitstring contains a bit set for each Ability you
     * want this enchantment to give to its attached character.
     * @param triggerPowers This map contains all trigger powers you want this
     * enchantment to give to its attached card.
     * @param maxHPModifier The max health modifier to apply to the attached
     * minion.
     *
     * If maximum health would go below 1, this kills the enchanted minion.
     *
     * When maximum health is increased by an enchantment, the character's
     * current health increases by the same amount.
     *
     * When maximum health is decreased by an enchantment or the removal of
     * enchantments, the actual health of the character will only decrease as
     * far as the new maximum.
     * @param atkModifier The attack modifier to apply to the attached
     * character. This can only make attack decrease as far as 0.
     * @param costModifier The mana cost modifier to apply to the attached
     * card. This can only make mana cost decrease as far as 0.
     * @param spellDmgModifier The damage modificator to apply to the attached
     * damaging spell.
     */
    Enchantment(bool temporary, const QString& name, const QString& desc,
                Ability abilities,
                const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                int maxHPModifier, int atkModifier, int costModifier,
                int spellDmgModifier);

    /**
     * @brief Copy constructor.
     *
     * This does not perform a deep copy of this enchantment but a shallow one,
     * unspecialized and with no buffedCard() nor sourceCard().
     *
     * @param e The enchantment to copy.
     */
    Enchantment(const Enchantment& e);

    /**
     * @brief Destructor.
     */
    virtual ~Enchantment();

    /**
     * @brief Gets the enchantment name that could possibly be displayed in a
     * UI
     * @return The enchantment name.
     */
    virtual QString name() const;

    /**
     * @brief Gets the card which created this enchantment. It can be a spell
     * like <i>Give +3 ATK to an ally minion</i>, but it can also have as
     * consequence something that enchant another character (or itself!).
     *
     * @return The card which created the enchantment, or \c NULL if this
     * enchantment was not created using the specialize() method of another
     * enchantment.
     *
     * @sa specialize()
     */
    virtual Card* sourceCard() const;

    /**
     * @brief Gets the card which benefits (or suffer) from this enchantment.
     *
     * @return The card which benefits (or suffer) from this enchantment, or \c
     * NULL if this enchantment was not created using the specialize() method
     * of another enchantment.
     *
     * @sa specialize()
     */
    virtual Card* buffedCard() const;

    /**
     * @brief Gets the abilities this enchantment gives to its attached
     * character.
     *
     * @return A bitstring containing a bit set for each Ability this
     * enchantment gives to its attached character.
     */
    virtual Ability abilities() const;

    /**
     * @brief Gets the trigger powers this enchantment gives to its attached
     * character.
     * @return The trigger powers this enchantment gives to its attached
     * character.
     */
    virtual const QHash<Event::Type, QVector<Trigger*>*>* triggerPowers() const;

    /**
     * @brief Gets the max health modifier this enchantment gives to its
     * attached character.
     *
     * If maximum health would go below 1, this kills the enchanted minion.
     *
     * When maximum health is increased by an enchantment, the character's
     * current health increases by the same amount.
     *
     * When maximum health is decreased by an enchantment or the removal of
     * enchantments, the actual health of the character will only decrease as
     * far as the new maximum.
     *
     * @return The max health modifier this enchantment gives to its attached
     * character.
     */
    virtual int maxHPModifier() const;

    /**
     * @brief Gets the attack modifier this enchantment gives to its attached
     * character.
     *
     * This can only make attack decrease as far as 0.
     *
     * @return The attack modifier this enchantment gives to its attached
     * character.
     */
    virtual int atkModifier() const;

    /**
     * @brief Gets the mana cost modifier this enchantment gives to its
     * attached character.
     *
     * This can only make the cost decrease as far as 0.
     *
     * @return The mana cost modifier this enchantment gives to its attached
     * character.
     */
    virtual int costModifier() const;

    /**
     * @brief Applies this enchantment to its target.
     *
     * This enchantment \e must be a specialized one, meaning it has to have
     * been created using the specialize() method of another Enchantment.
     *
     * @sa specialize()
     */
    virtual void apply();

    /**
     * @brief Specializes this enchantment with a target to enchant and a Card
     * initiating its creation.
     *
     * As Action's, an Enchantment object represents both a flyweight factory
     * and a flyweight itself. At the moment a card enchants another one, the
     * Enchantment object it has as reference is specialized using the
     * specialize() method. This clones (deep copy) the reference enchantment
     * and sets its buffedCard() and its sourceCard(), leaving the reference
     * untouched for future reuse in case it could enchant again another card.
     *
     * This method also takes care of specializing its trigger powers (if any)
     * for its target.
     *
     * @warning You cannot specialize() an already specialized enchantment.
     *
     * @param sourceCard The card issuing the enchantment
     * @param buffedCard The card receiving the enchantment
     * @return A deep copy of this enchantment, specialized for \a sourceCard
     * and \a buffedCard.
     */
    virtual Enchantment* specialize(Card* sourceCard, Card* buffedCard);

protected:
    Card* m_source;
    Card* m_buffedCard;
    bool m_temporary;
    QString m_name;
    QString m_desc;
    Ability m_abilities;
    QHash<Event::Type, QVector<Trigger*>*> m_triggerPowers;

    int m_maxHPModifier;
    int m_atkModifier;
    int m_costModifier;
    int m_spellDmgModifier;
    bool m_specialized;


    // Stringable interface
public:
    /**
     * @brief Gets this enchantment QString representation.
     *
     * @return The description of this enchantment.
     */
    QString toString() const;
};

#endif // ENCHANTMENT_H
