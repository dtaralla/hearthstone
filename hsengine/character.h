#ifndef CHARACTER_H
#define CHARACTER_H

#include "card.h"
#include "characterinfo.h"

class Enchantment;

class Character : public Card
{
public:
    Character(CharacterInfo const* baseChar, Player* owner = NULL);
    virtual ~Character();

    virtual const CardIdentity* base() const;
    virtual CardType type() const;
    virtual CharacterType characterType() const;
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
    CharacterInfo const* m_base;
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
