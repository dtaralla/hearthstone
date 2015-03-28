#ifndef TARGETTHISEXPRESSION_H
#define TARGETTHISEXPRESSION_H

#include "targetexpression.h"

class TargetThisExpression : public TargetExpression
{
public:
    TargetThisExpression();
    ~TargetThisExpression();

    // Stringable interface
public:
    QString toString() const;

    // TargetExpression interface
public:
    QVector<Character*>* eval(Card* caster, Action* forAction, const Event* e = NULL);
    bool noTargets(Card* caster, const Event* e = NULL) const;
    bool selectionRequired() const;
};

#endif // TARGETTHISEXPRESSION_H
