#ifndef TURNENDTRIGGER_H
#define TURNENDTRIGGER_H

#include "trigger.h"

class TurnEndTrigger : public Trigger
{
public:
    TurnEndTrigger(QVector<Action*>* actions,
                   Owner playerEndingTurn = Owners::ANY_OWNER);
    TurnEndTrigger(Action* action, Owner playerEndingTurn = Owners::ANY_OWNER);
    virtual ~TurnEndTrigger();

private:
    Owner m_playerEndingTurn;

    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // TURNENDTRIGGER_H
