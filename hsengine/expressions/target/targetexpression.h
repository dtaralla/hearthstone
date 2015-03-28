#ifndef TARGETEXPRESSION_H
#define TARGETEXPRESSION_H

#include "stringable.h"
#include <QVector>

class Card;
class Character;
class Event;
class Action;

class TargetExpression : public Stringable
{
public:
    virtual ~TargetExpression();
    virtual QVector<Character*>* eval(Card* caster, Action* forAction, const Event* e = NULL) = 0;
    virtual bool noTargets(Card* caster, const Event* e = NULL) const = 0;
    virtual bool selectionRequired() const = 0;
    virtual bool canBeHero() const;

};

#endif // TARGETEXPRESSION_H
