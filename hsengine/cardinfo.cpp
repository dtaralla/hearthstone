#include "cardinfo.h"
#include "triggers/trigger.h"

CardInfo::CardInfo(int id, const QString& name, const QString& desc, int manaCost,
                   const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers, bool collectible):
    m_id(id),
    m_name(name),
    m_desc(desc),
    m_manaCost(manaCost),
    m_triggerPowers(triggerPowers),
    m_collectible(collectible)
{

}

CardInfo::~CardInfo()
{
    foreach (QVector<Trigger*>* va, m_triggerPowers.values()) {
        qDeleteAll(*va);
        delete va;
    }
}

int CardInfo::id() const
{
    return m_id;
}

const QString& CardInfo::name() const
{
    return m_name;
}

const QString& CardInfo::desc() const
{
    return m_desc;
}

int CardInfo::manaCost() const
{
    return m_manaCost;
}

bool CardInfo::listensTo(const Event& e) const
{
    return m_triggerPowers.contains(e.type());
}

const QHash<Event::Type, QVector<Trigger*>* >& CardInfo::triggerPowers() const
{
    return m_triggerPowers;
}

bool CardInfo::collectible() const
{
    return m_collectible;
}



QDebug operator<<(QDebug dbg, const CardInfo& c)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << c.name();
    return dbg;
}

