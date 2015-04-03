#ifndef SILENCEACTION_H
#define SILENCEACTION_H

#include "targetedaction.h"

class SilenceTargetAction : public TargetedAction
{
public:
    SilenceTargetAction(int id, const QSharedPointer<TargetExpression>& target);
    ~SilenceTargetAction();

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e = NULL);
    SilenceTargetAction* cloneFlyweight() const;
};

#endif // SILENCEACTION_H
