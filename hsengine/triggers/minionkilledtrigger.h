#ifndef MINIONKILLEDTRIGGER_H
#define MINIONKILLEDTRIGGER_H

#include "trigger.h"

class MinionKilledTrigger : public Trigger
{
public:
    MinionKilledTrigger(QVector<Action*>* actions,
                        Owner owningPlayer = Owners::ANY_OWNER,
                        CharacterType minionType = CharacterTypes::MINION);
    MinionKilledTrigger(Action* action,
                        Owner owningPlayer = Owners::ANY_OWNER,
                        CharacterType minionType = CharacterTypes::MINION);
    virtual ~MinionKilledTrigger();

private:
    Owner m_owningPlayer;
    CharacterType m_minionType;


    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // MINIONKILLEDTRIGGER_H
