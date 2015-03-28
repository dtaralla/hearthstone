#include "heroinfo.h"
#include "actions/action.h"
#include "actions/specialpoweraction.h"

HeroInfo::HeroInfo(int id, const QString& name, SpecialPowerAction* specialPower) :
    CharacterInfo(id, name, "", 0, 30, 0, QVector<Action*>(), QVector<Action*>(),
                  Abilities::NO_ABILITY,
                  QHash<Event::Type, QVector<Trigger*>* >(), CharacterTypes::HERO, false),
    m_specialPower(specialPower)
{

}

HeroInfo::~HeroInfo()
{
    delete m_specialPower;
}

SpecialPowerAction* HeroInfo::specialPower() const
{
    return m_specialPower;
}

