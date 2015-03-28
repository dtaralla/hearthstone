#ifndef SILENCEENCHANTMENT_H
#define SILENCEENCHANTMENT_H

#include "enchantment.h"

class SilenceEnchantment : public Enchantment
{
public:
    SilenceEnchantment(Card* sourceCard, Card* buffedCard);
    ~SilenceEnchantment();

    void apply();
    QString toString() const;

private:
    QVector<Enchantment*> m_silencedEnchantments;
    bool m_applied;

    Ability abilities() const;
    const QHash<Event::Type, QVector<Trigger*>*>* triggerPowers() const;
    int maxHPModifier() const;
    int atkModifier() const;
    int costModifier() const;
};

#endif // SILENCEENCHANTMENT_H
