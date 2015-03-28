#ifndef SPELL_H
#define SPELL_H

#include "card.h"

class SpellInfo;

class Spell : public Card
{
public:
    Spell(CardInfo const* baseSpell, Player* owner = NULL);
    ~Spell();

    PlayAction* playAction();
    const QVector<Action*>* playActionConsequences() const;

    const CardInfo* base() const;
    CardType type() const;
    void initCard(Player* owner);

    void addDmgModifier(int mod);
    int dmgModifier() const;
    void clearEnchantmentEffects();

private:
    SpellInfo const* m_base;
    PlayAction* m_playAction;
    QVector<Action*>* m_spell;
    int m_dmgModifier;
};

#endif // SPELL_H
