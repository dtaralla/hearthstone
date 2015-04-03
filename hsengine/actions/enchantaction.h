#ifndef ENCHANTACTION_H
#define ENCHANTACTION_H

#include "targetedaction.h"

class Enchantment;

class EnchantAction : public TargetedAction
{
public:
    EnchantAction(int id, const QSharedPointer<TargetExpression>& target, const QSharedPointer<Enchantment>& e);
    ~EnchantAction();
    const Enchantment* enchantment() const;

private:
    QSharedPointer<Enchantment> m_enchantment;


    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    ActionType type() const;
    void resolve(const Event* e = NULL);
    EnchantAction* cloneFlyweight() const;
};

#endif // ENCHANTACTION_H
