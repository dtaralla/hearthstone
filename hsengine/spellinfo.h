#ifndef SPELLINFO_H
#define SPELLINFO_H

#include <QHash>
#include "cardinfo.h"
#include "event.h"

class Action;

class SpellInfo : public CardInfo
{
public:
    SpellInfo(int id, const QString& name, const QString& desc,
              int manaCost, QVector<Action*>* spell,
              const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers, bool collectible);
    ~SpellInfo();

    CardType type() const;
    QVector<Action*>* spell() const;

private:
    QVector<Action*>* m_spell;
};

#endif // SPELLINFO_H
