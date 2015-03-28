#ifndef SUMMONMINIONACTION_H
#define SUMMONMINIONACTION_H

#include "action.h"
class Minion;

class SummonMinionAction : public Action
{
public:
    SummonMinionAction(Minion* c);

    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;
    bool resolvable(const Event* e = NULL) const;

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    Action* setSourceCard(Card* c) const;
};

#endif // SUMMONMINIONACTION_H
