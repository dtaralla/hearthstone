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

    CharacterDamagedTrigger(QVector<Action*>* actions, TargetExpression* damagers = NULL,
                            TargetExpression* damagedCharacters = NULL,
                            SourceType accepedSources = ANY);
    CharacterDamagedTrigger(Action* action, TargetExpression* damager = NULL,
                            TargetExpression* damagedCharacter = NULL,
                            SourceType accepedSources = ANY);
    ~CharacterDamagedTrigger();

private:
    QSharedPointer<TargetExpression> m_damagers;
    QSharedPointer<TargetExpression> m_damagedCharacters;
    SourceType m_acceptedSources;


    // Trigger interface
public:
    bool listensTo(const Event& e) const;
    Trigger* clone() const;
};

#endif // CHARACTERDAMAGEDTRIGGER_H
