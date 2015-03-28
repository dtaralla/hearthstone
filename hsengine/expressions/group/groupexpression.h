#ifndef GROUPEXPRESSION_H
#define GROUPEXPRESSION_H

#include "stringable.h"
#include "game_globals.h"
#include "event.h"
#include <QSharedPointer>

class TargetExpression;
class Character;
class Card;

class GroupExpression : public Stringable
{
public:
    GroupExpression(Owner fromPlayer, CharacterType subtype, const QSharedPointer<TargetExpression>& excludedTargets);
    virtual ~GroupExpression();

    virtual QVector<Character*>* eval(Card* caster, const Event* e = NULL) const;
    virtual bool isEmpty(Card* caster, const Event* e = NULL) const;
    virtual CharacterType acceptedSubtypes() const;
    virtual Owner owner() const;

protected:
    Owner m_owner;
    CharacterType m_type;
    QSharedPointer<TargetExpression> m_excludedTargets;

    // Stringable interface
public:
    virtual QString toString() const;
};

#endif // GROUPEXPRESSION_H
