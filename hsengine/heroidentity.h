#ifndef HEROIDENTITY_H
#define HEROIDENTITY_H

#include "characteridentity.h"

class SpecialPowerAction;

class HeroIdentity : public CharacterIdentity
{
public:
    HeroIdentity(int id, const QString& name,
             SpecialPowerAction* specialPower);
    ~HeroIdentity();

    SpecialPowerAction* specialPower() const;

private:
    SpecialPowerAction* m_specialPower;
};

#endif // HEROIDENTITY_H
