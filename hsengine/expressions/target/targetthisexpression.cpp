#include "targetthisexpression.h"
#include "card.h"

TargetThisExpression::TargetThisExpression()
{

}

TargetThisExpression::~TargetThisExpression()
{

}



QString TargetThisExpression::toString() const
{
    return "(this character)";
}

QVector<Character*>* TargetThisExpression::eval(Card* caster, Action* forAction, const Event* e)
{
    Q_UNUSED(e); Q_UNUSED(forAction);

    if (!(caster->type() & CardTypes::CARD_CHARACTER))
        qCritical() << *caster << "uses (this) target, but (this) is not a character and thus is not a valid target";

    return new QVector<Character*>(1, (Character*) caster);
}

bool TargetThisExpression::noTargets(Card* caster, const Event* e) const
{
    Q_UNUSED(e);
    return caster->type() & CardTypes::CARD_CHARACTER;
}

bool TargetThisExpression::selectionRequired() const
{
    return false;
}
