#ifndef DAMAGEANDFREEZEACTION_H
#define DAMAGEANDFREEZEACTION_H

#include "dealdmghealingaction.h"

class TargetExpression;
class FreezeAction;

class DamageAndFreezeAction : public DealDmgHealingAction
{
public:
    DamageAndFreezeAction(int id, const QSharedPointer<TargetExpression>& target, int quantity);
    ~DamageAndFreezeAction();

private:
    FreezeAction* m_freezeAction;

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e);
    DamageAndFreezeAction* cloneFlyweight() const;
};

#endif // DAMAGEANDFREEZEACTION_H
