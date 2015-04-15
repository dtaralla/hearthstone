#include "spellidentity.h"
#include "actions/action.h"

SpellIdentity::SpellIdentity(int id, const QString& name,
                     const QString& desc, int manaCost, QVector<Action*>* spell,
                     const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
                     bool collectible = true) :
    CardIdentity(id, name, desc, manaCost, triggerPowers, collectible),
    m_spell(spell)
{

}

SpellIdentity::~SpellIdentity()
{
    qDeleteAll(*m_spell);
    delete m_spell;
}

QVector<Action*>* SpellIdentity::spell() const
{
    return m_spell;
}


CardType SpellIdentity::type() const
{
    return CardTypes::CARD_SPELL;
}
