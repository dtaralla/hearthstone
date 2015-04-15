#include "characteridentity.h"
#include "cardidentity.h"
#include "actions/action.h"
#include "triggers/trigger.h"

CharacterIdentity::CharacterIdentity(int id, const QString& name,
                             const QString& desc, int manaCost, int hp, int atk,
                             const QVector<Action*>& battlecry,
                             const QVector<Action*>& deathrattle, Ability abilities,
                             const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                             CharacterType type, bool collectible) :
    CardIdentity(id, name, desc, manaCost, triggerPowers, collectible),
    m_type(type),
    m_hp(hp),
    m_atk(atk),
    m_battlecry(battlecry),
    m_deathrattle(deathrattle),
    m_abilities(abilities)
{
    if (m_type == CharacterTypes::CHARACTER || m_type == CharacterTypes::MINION) {
        qDebug() << "Character type" << CharacterTypes::toString(m_type)
                 << "is forbidden; defaulting to"
                 << CharacterTypes::toString(CharacterTypes::GENERAL);
        m_type = CharacterTypes::GENERAL;
    }
}

CharacterIdentity::~CharacterIdentity()
{
    qDeleteAll(m_deathrattle);
    qDeleteAll(m_battlecry);
}

CharacterType CharacterIdentity::subtype() const
{
    return m_type;
}

CardType CharacterIdentity::type() const
{
    if (m_type & CharacterTypes::HERO)
        return CardTypes::CARD_HERO;

    return CardTypes::CARD_MINION;
}


int CharacterIdentity::hp() const
{
    return m_hp;
}

int CharacterIdentity::atk() const
{
    return m_atk;
}

bool CharacterIdentity::hasAbility(Ability a) const
{
    return m_abilities & a;
}

const QVector<Action*>*CharacterIdentity::battlecry() const
{
    return &m_battlecry;
}

const QVector<Action*>* CharacterIdentity::deathRattles() const
{
    return &m_deathrattle;
}
