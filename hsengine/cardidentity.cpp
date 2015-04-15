#include "cardidentity.h"
#include "triggers/trigger.h"

CardIdentity::CardIdentity(int id, const QString& name, const QString& desc, int manaCost,
                   const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers, bool collectible):
    m_id(id),
    m_name(name),
    m_desc(desc),
    m_manaCost(manaCost),
    m_triggerPowers(triggerPowers),
    m_collectible(collectible)
{

}

CardIdentity::~CardIdentity()
{
    foreach (QVector<Trigger*>* va, m_triggerPowers.values()) {
        qDeleteAll(*va);
        delete va;
    }
}

int CardIdentity::id() const
{
    return m_id;
}

const QString& CardIdentity::name() const
{
    return m_name;
}

const QString& CardIdentity::desc() const
{
    return m_desc;
}

int CardIdentity::manaCost() const
{
    return m_manaCost;
}

const QHash<Event::Type, QVector<Trigger*>* >& CardIdentity::triggerPowers() const
{
    return m_triggerPowers;
}

bool CardIdentity::collectible() const
{
    return m_collectible;
}



QDebug operator<<(QDebug dbg, const CardIdentity& c)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << c.name();
    return dbg;
}

