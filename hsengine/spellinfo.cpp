#include "spellinfo.h"
#include "actions/action.h"

SpellInfo::SpellInfo(int id, const QString& name,
                     const QString& desc, int manaCost, QVector<Action*>* spell,
                     const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
                     bool collectible = true) :
    CardIdentity(id, name, desc, manaCost, triggerPowers, collectible),
    m_spell(spell)
{

}

SpellInfo::~SpellInfo()
{
    qDeleteAll(*m_spell);
    delete m_spell;
}

QVector<Action*>* SpellInfo::spell() const
{
    return m_spell;
}


CardType SpellInfo::type() const
{
    return CardTypes::CARD_SPELL;
}
