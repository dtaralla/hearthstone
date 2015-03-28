#ifndef HEROINFO_H
#define HEROINFO_H

#include "characterinfo.h"

class SpecialPowerAction;

class HeroInfo : public CharacterInfo
{
public:
    HeroInfo(int id, const QString& name,
             SpecialPowerAction* specialPower);
    ~HeroInfo();

    SpecialPowerAction* specialPower() const;

private:
    SpecialPowerAction* m_specialPower;
};

#endif // HEROINFO_H
