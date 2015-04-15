#ifndef SPELL_H
#define SPELL_H

#include "card.h"

class SpellIdentity;

class Spell : public Card
{
public:
    Spell(CardIdentity const* baseSpell, Player* owner = NULL);
    ~Spell();

    PlayAction* playAction();
    const QVector<Action*>* playActionConsequences() const;

    const CardIdentity* base() const;
    CardType type() const;
    void initCard(Player* owner);

    void addDmgModifier(int mod);
    int dmgModifier() const;
    void clearEnchantmentEffects();

private:
    SpellIdentity const* m_base;
    PlayAction* m_playAction;
    QVector<Action*>* m_spell;
    int m_dmgModifier;
};

#endif // SPELL_H
