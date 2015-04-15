#include "heroidentity.h"
#include "actions/action.h"
#include "actions/specialpoweraction.h"

HeroIdentity::HeroIdentity(int id, const QString& name, SpecialPowerAction* specialPower) :
    CharacterIdentity(id, name, "", 0, 30, 0, QVector<Action*>(), QVector<Action*>(),
                  Abilities::NO_ABILITY,
                  QHash<Event::Type, QVector<Trigger*>* >(), CharacterTypes::HERO, false),
    m_specialPower(specialPower)
{

}

HeroIdentity::~HeroIdentity()
{
    delete m_specialPower;
}

SpecialPowerAction* HeroIdentity::specialPower() const
{
    return m_specialPower;
}

