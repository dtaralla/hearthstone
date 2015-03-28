#ifndef ENCHANTMENT_H
#define ENCHANTMENT_H

#include "stringable.h"
#include "game_globals.h"
#include "event.h"
#include <QHash>

class Trigger;
class Card;


class Enchantment : public Stringable
{
public:
    Enchantment(bool temporary, const QString& name, const QString& desc,
                Ability abilities,
                const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                int maxHPModifier, int atkModifier, int costModifier,
                int spellDmgModifier);
    Enchantment(const Enchantment& e);
    virtual ~Enchantment();

    virtual QString name() const;
    virtual Card* sourceCard() const;
    virtual Card* buffedCard() const;
    virtual Ability abilities() const;
    virtual const QHash<Event::Type, QVector<Trigger*>*>* triggerPowers() const;
    virtual int maxHPModifier() const;
    virtual int atkModifier() const;
    virtual int costModifier() const;
    virtual void apply();
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
    QString toString() const;
};

#endif // ENCHANTMENT_H
