#ifndef TARGETEDACTION_H
#define TARGETEDACTION_H

#include "action.h"
#include <QSharedPointer>

class TargetExpression;
class Character;

class TargetedAction : public Action
{
public:
    TargetedAction(int id, const QSharedPointer<TargetExpression>& target);
    virtual ~TargetedAction();

    const TargetExpression* target() const;
    virtual const QVector<Character*>* lastTargets() const;
    virtual void forceTargets(const QVector<Character*>& targets);
    virtual int id() const;

private:
    QVector<Character*> m_forcedTargets;

protected:
    const int m_id;
    QSharedPointer<TargetExpression> m_target;
    QVector<Character*> m_lastTargets;

    QVector<Character*>* m_targets(Card* caster, const Event* e = NULL);
    void m_refreshLastTargets(const QVector<Character*>* lastTargets);

    // Action interface
public:
    virtual bool resolvable(const Event *e = NULL) const;
    virtual bool isTargetedAction() const;
};

#endif // TARGETEDACTION_H
