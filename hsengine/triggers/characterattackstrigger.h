#ifndef CHARACTERATTACKSTRIGGER_H
#define CHARACTERATTACKSTRIGGER_H

#include "trigger.h"
#include <QSharedPointer>

class TargetExpression;

class CharacterAttacksTrigger : public Trigger
{
public:
    CharacterAttacksTrigger(QVector<Action*>* actions, const QSharedPointer<TargetExpression>& attakers,
                            const QSharedPointer<TargetExpression>& attackedCharacters);
    CharacterAttacksTrigger(Action* action, const QSharedPointer<TargetExpression>& attakers,
                            const QSharedPointer<TargetExpression>& attackedCharacters);
    ~CharacterAttacksTrigger();

private:
    QSharedPointer<TargetExpression> mSources;
    QSharedPointer<TargetExpression> mTargets;


    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // CHARACTERATTACKSTRIGGER_H
