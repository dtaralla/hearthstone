#ifndef DAMAGINGSPELLACTION_H
#define DAMAGINGSPELLACTION_H

#include "targetedaction.h"

class DamagingSpellAction : public TargetedAction
{
public:
    DamagingSpellAction(int id, const QSharedPointer<TargetExpression>& target, int quantity);
    virtual ~DamagingSpellAction();
    virtual void addSpellDmg(int quantity);
    virtual int quantity() const;

protected:
    int m_quantity;

};

#endif // DAMAGINGSPELLACTION_H
