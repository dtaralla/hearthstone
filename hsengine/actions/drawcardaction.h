#ifndef DRAWCARDACTION_H
#define DRAWCARDACTION_H

#include "actions/action.h"

class DrawCardAction : public Action
{
public:
    DrawCardAction(int quantity = 1, Owner targetOwner = Owners::ALLY);
    ~DrawCardAction();

    int quantity() const;
    Owner targetOwner() const;

private:
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

#endif // DRAWCARDACTION_H
