#include "characterinfo.h"
#include "cardinfo.h"
#include "actions/action.h"
#include "triggers/trigger.h"

CharacterInfo::CharacterInfo(int id, const QString& name,
                             const QString& desc, int manaCost, int hp, int atk,
                             const QVector<Action*>& battlecry,
                             const QVector<Action*>& deathRattles, Ability abilities,
                             const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                             CharacterType type, bool collectible) :
    CardInfo(id, name, desc, manaCost, triggerPowers, collectible),
    m_type(type),
    m_hp(hp),
    m_atk(atk),
    m_battlecry(battlecry),
    m_deathRattles(deathRattles),
    m_abilities(abilities)
{

}

CharacterInfo::~CharacterInfo()
{
    qDeleteAll(m_deathRattles);
    qDeleteAll(m_battlecry);
}

CharacterType CharacterInfo::characterType() const
{
    return m_type;
}

CardType CharacterInfo::type() const
{
    if (m_type & CharacterTypes::HERO)
        return CardTypes::CARD_HERO;

    return CardTypes::CARD_MINION;
}


int CharacterInfo::hp() const
{
    return m_hp;
}

int CharacterInfo::atk() const
{
    return m_atk;
}

bool CharacterInfo::hasAbility(Ability a) const
{
    return m_abilities & a;
}

const QVector<Action*>*CharacterInfo::battlecry() const
{
    return &m_battlecry;
}

const QVector<Action*>* CharacterInfo::deathRattles() const
{
    return &m_deathRattles;
}
