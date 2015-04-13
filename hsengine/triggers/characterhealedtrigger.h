#ifndef CHARACTERHEALEDTRIGGER_H
#define CHARACTERHEALEDTRIGGER_H

#include "trigger.h"
#include <QSharedPointer>

class TargetExpression;

class CharacterHealedTrigger : public Trigger
{
public:
    enum SourceType {
        ANY,
        CHARACTER,
        HERO,
        MINION,
        SPELL
    };

    CharacterHealedTrigger(QVector<Action*>* actions, const QSharedPointer<TargetExpression>& healers,
                            const QSharedPointer<TargetExpression>& healedCharacters,
                            SourceType accepedSources = ANY);
    CharacterHealedTrigger(Action* action, const QSharedPointer<TargetExpression>& healers,
                            const QSharedPointer<TargetExpression>& healedCharacters,
                            SourceType accepedSources = ANY);
    ~CharacterHealedTrigger();

private:
    QSharedPointer<TargetExpression> mSources;
    QSharedPointer<TargetExpression> mTargets;
    SourceType mAcceptedSourceType;


    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // CHARACTERHEALEDTRIGGER_H
