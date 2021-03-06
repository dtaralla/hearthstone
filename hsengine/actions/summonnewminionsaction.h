#ifndef SUMMONNEWMINIONSACTION_H
#define SUMMONNEWMINIONSACTION_H

#include "action.h"

class CharacterIdentity;

class SummonNewMinionsAction : public Action
{
public:
    SummonNewMinionsAction(int id, int quantity = 1, Owner targetOwner = Owners::ALLY);
    ~SummonNewMinionsAction();

    const CharacterIdentity* base() const;
    int quantity() const;
    Owner targetOwner() const;

private:
    const CharacterIdentity* m_base;
    int m_quantity;
    Owner m_targetOwner;

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;
};

#endif // SUMMONNEWMINIONSACTION_H
