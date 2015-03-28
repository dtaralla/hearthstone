#ifndef EVENTCHARACTERPARAMEXPRESSION_H
#define EVENTCHARACTERPARAMEXPRESSION_H

#include "targetexpression.h"
#include "event.h"

class EventCharacterParamExpression : public TargetExpression
{
public:
    EventCharacterParamExpression(Event::Type type, const QVector<EventParam>& toGet);
    ~EventCharacterParamExpression();

private:
    Event::Type m_type;
    QVector<EventParam> m_toGet;

    // Stringable interface
public:
    QString toString() const;

    // TargetExpression interface
public:
    QVector<Character*>* eval(Card* caster, Action* forAction, const Event* e);
    bool noTargets(Card* caster, const Event* e) const;
    bool selectionRequired() const;
};

#endif // EVENTCHARACTERPARAMEXPRESSION_H
