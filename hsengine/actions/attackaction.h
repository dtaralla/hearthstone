#ifndef ATTACKACTION_H
#define ATTACKACTION_H

#include "action.h"

class Character;

class AttackAction : public Action
{
public:
    AttackAction(Character* c);

    virtual QString toString() const;
    ActionType type() const;
    void resolve(const Event* e = NULL);
    AttackAction* cloneFlyweight() const;
    Character* lastTarget() const;
    int id() const;

private:
    Character* m_lastTarget;

    // Action interface
public:
    Action* setSourceCard(Card* c) const;
    bool isTargetedAction() const;
};

#endif // ATTACKACTION_H
