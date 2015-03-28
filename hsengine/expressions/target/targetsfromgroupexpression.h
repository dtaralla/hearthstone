#ifndef TARGETSFROMGROUPEXPRESSION_H
#define TARGETSFROMGROUPEXPRESSION_H

#include "targetexpression.h"
#include "game_globals.h"
#include "expressions/group/groupexpression.h"

namespace PickingTypes {
    enum PickingType {
        RANDOM,
        USER_SELECT,
        ALL
    };
}

class TargetsFromGroupExpression : public TargetExpression
{
public:
    TargetsFromGroupExpression(PickingTypes::PickingType type, int quantity,
                               GroupExpression fromGroup, bool withReplacement = false);
    ~TargetsFromGroupExpression();
    virtual bool noTargets(Card* caster, const Event* e = NULL) const;
    Owner owner() const;
    int howManySelections() const;
    PickingTypes::PickingType pickingType() const;
    bool withReplacement() const;

private:
    PickingTypes::PickingType m_type;
    int m_quantity;
    GroupExpression m_group;
    bool m_withReplacement;

    // Stringable interface
public:
    QString toString() const;

    // TargetExpression interface
public:
    QVector<Character*>* eval(Card* caster, Action* forAction, const Event* e = NULL);
    bool selectionRequired() const;
    bool canBeHero() const;
};

#endif // TARGETSFROMGROUPEXPRESSION_H
