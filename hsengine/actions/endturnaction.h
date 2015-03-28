#ifndef ENDTURNACTION_H
#define ENDTURNACTION_H

#include "action.h"

class Game;

class EndTurnAction : public Action
{
public:
    EndTurnAction(Game* g);
    ~EndTurnAction();

    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    Action* setSourceCard(Card* c) const;

private:
    Game* mGame;
};

#endif // ENDTURNACTION_H
