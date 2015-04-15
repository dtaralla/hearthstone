#ifndef SPELLIDENTITY_H
#define SPELLIDENTITY_H

#include <QHash>
#include "cardidentity.h"
#include "event.h"

class Action;

class SpellIdentity : public CardIdentity
{
public:
    SpellIdentity(int id, const QString& name, const QString& desc,
              int manaCost, QVector<Action*>* spell,
              const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers, bool collectible);
    ~SpellIdentity();

    CardType type() const;
    QVector<Action*>* spell() const;

private:
    QVector<Action*>* m_spell;
};

#endif // SPELLIDENTITY_H
