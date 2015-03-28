#ifndef FREEZEACTION_H
#define FREEZEACTION_H

#include "targettedaction.h"

class TargetExpression;

class FreezeAction : public TargetedAction
{
public:
    FreezeAction(int id, const QSharedPointer<TargetExpression>& target);
    ~FreezeAction();

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e);
    FreezeAction* cloneFlyweight() const;
};

#endif // FREEZEACTION_H
