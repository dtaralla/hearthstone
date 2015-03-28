#ifndef PLAYACTION_H
#define PLAYACTION_H

#include "action.h"

class PlayAction : public Action
{
public:
    PlayAction(Card* c);
    ~PlayAction();

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;
    bool resolvable(const Event* e = NULL) const;
    Action* setSourceCard(Card* c) const;
};

#endif // PLAYACTION_H
