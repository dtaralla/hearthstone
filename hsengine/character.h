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
     * @brief attackAction
     * @return
     */
    virtual Action* attackAction() const;
    virtual bool hasAttacked() const;
    virtual void setHasAttacked(bool hasAttacked);
    virtual int maxHp() const;
    virtual int hp() const;
    virtual int atk() const;
    virtual bool dying() const;
    virtual int triggerPowersCount() const;
    virtual void dealDmg(Card* dmgSource, int dmg);
    virtual void heal(Card* healingSource, int healing);
    virtual void modifyHp(int modification);
    virtual void addMaxHPModifier(int modification);
    virtual void addAtkModifier(int modification);
    virtual void addAbilities(Ability a);
    virtual void setDying(bool dying);
    virtual bool injured() const;
    virtual bool hasAbility(Ability a) const;
    QVector<Enchantment*>* takeEnchantmentsBefore(const Enchantment* s);
    virtual void removeEnchantment(Enchantment* e);
    virtual void clearEnchantmentEffects();
    virtual void setSilenced(bool silenced);
    virtual bool isSilenced() const;
    virtual bool listensTo(const Event &e) const;
    virtual void freeze(Card* sourceCard);
    virtual void unfreeze();

protected:
    CharacterIdentity const* m_base;
    Ability m_additionalAbilities;
    Action* m_attackAction;

    int m_maxHpModifier;
    int m_curHp;
    int m_atkModifier;

    bool m_hasAttacked;
    bool m_dying;
    bool m_silenced;
    Enchantment* m_frozenEnchantment;
};

#endif // CHARACTER_H
