#ifndef ADDMANAACTION_H
#define ADDMANAACTION_H

#include "action.h"

class AddManaAction : public Action
{
public:
    AddManaAction(Owner target, int quantity, bool persistent = false);
    ~AddManaAction();

    int quantity() const;
    Owner targetOwner() const;

    virtual QString toString() const;
    ActionType type() const;
    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;

private:
    Owner m_target;
    int m_quantity;
    bool m_persistent;
};

#endif // ADDMANAACTION_H
