#ifndef DEALDMGHEALINGACTION_H
#define DEALDMGHEALINGACTION_H

#include "damagingspellaction.h"

class TargetExpression;

class DealDmgHealingAction : public DamagingSpellAction
{
public:
    DealDmgHealingAction(int id, const QSharedPointer<TargetExpression>& target, int quantity);
    ~DealDmgHealingAction();

    ActionType type() const;
    void resolve(const Event* e = NULL);
    DealDmgHealingAction* cloneFlyweight() const;


    // Stringable interface
public:
    QString toString() const;
};

#endif // DEALDMGHEALINGACTION_H
