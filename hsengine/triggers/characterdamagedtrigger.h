#ifndef CHARACTERDAMAGEDTRIGGER_H
#define CHARACTERDAMAGEDTRIGGER_H

#include "trigger.h"
#include <QSharedPointer>

class TargetExpression;

class CharacterDamagedTrigger : public Trigger
{
public:
    enum SourceType {
        ANY,
        CHARACTER,
        HERO,
        MINION,
        SPELL
    };

    CharacterDamagedTrigger(QVector<Action*>* actions, const QSharedPointer<TargetExpression>& damagers,
                            const QSharedPointer<TargetExpression>& damagedCharacters,
                            SourceType accepedSources = ANY);
    CharacterDamagedTrigger(Action* action, const QSharedPointer<TargetExpression>& damagers,
                            const QSharedPointer<TargetExpression>& damagedCharacters,
                            SourceType accepedSources = ANY);
    ~CharacterDamagedTrigger();

private:
    QSharedPointer<TargetExpression> mSources;
    QSharedPointer<TargetExpression> mTargets;
    SourceType mAcceptedSourceType;


    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // CHARACTERDAMAGEDTRIGGER_H
