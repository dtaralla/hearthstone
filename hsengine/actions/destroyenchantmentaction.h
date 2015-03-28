#ifndef DESTROYENCHANTMENTACTION_H
#define DESTROYENCHANTMENTACTION_H

#include "action.h"

class Enchantment;

class DestroyEnchantmentAction : public Action
{
public:
    DestroyEnchantmentAction(Enchantment* toDestroy);
    ~DestroyEnchantmentAction();

private:
    Enchantment* m_toDestroy;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;
    QString toString() const;
    virtual bool deletesItselfAfterResolve() const;
};

#endif // DESTROYENCHANTMENTACTION_H
