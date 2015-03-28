#ifndef HERO_H
#define HERO_H

#include "character.h"

class HeroInfo;
class SpecialPowerAction;

class Hero : public Character
{
public:
    Hero(const HeroInfo* baseChar, Player* owner);
    ~Hero();

    PlayAction* playAction();
    QVector<Action*>* playActionConsequences() const;
    SpecialPowerAction* specialPower() const;

    virtual CardType type() const;
    virtual CharacterType characterType() const;
    void initCard(Player* owner);

    int armor() const;
    bool hasUsedSpecialPower() const;
    void setHasUsedSpecialPower(bool hasUsedSpecialPower);
    void modifyHp(int modification);

    void addArmor(int quantity);

private:
    int m_armor;
    PlayAction* m_playAction;
    SpecialPowerAction* m_specialPower;

};

#endif // HERO_H
