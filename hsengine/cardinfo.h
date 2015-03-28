#ifndef CARDINFO_H
#define CARDINFO_H

#include <QString>
#include <QtDebug>
#include <QHash>
#include <QVector>
#include "event.h"
#include "game_globals.h"

class Trigger;

class CardInfo
{
public:
    CardInfo(int id, const QString& name, const QString& desc, int manaCost,
             const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
             bool collectible = true);
    virtual ~CardInfo();

    virtual CardType type() const = 0;

    virtual int id() const;
    virtual const QString& name() const;
    virtual const QString& desc() const;
    virtual int manaCost() const;
    virtual bool listensTo(const Event& e) const;
    virtual const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers() const;
    virtual bool collectible() const;

protected:
    int m_id;
    QString m_name;
    QString m_desc;
    int m_manaCost;
    QHash<Event::Type, QVector<Trigger*>* > m_triggerPowers;
    bool m_collectible;
};

QDebug operator<<(QDebug dbg, const CardInfo& c);

#endif // CARDINFO_H
