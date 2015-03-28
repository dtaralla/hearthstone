#include "targetsfromgroupexpression.h"
#include "card.h"
#include "player.h"

TargetsFromGroupExpression::TargetsFromGroupExpression(PickingTypes::PickingType type,
                                                       int quantity,
                                                       GroupExpression fromGroup, bool withReplacement) :
    m_type(type),
    m_quantity(quantity),
    m_group(fromGroup),
    m_withReplacement(withReplacement)
{
    if (m_type == PickingTypes::USER_SELECT && m_quantity != 1) {
        qWarning() << "USER SELECT always ignores 'quantity' argument";
        m_quantity = 1;
    }
}

TargetsFromGroupExpression::~TargetsFromGroupExpression()
{

}

bool TargetsFromGroupExpression::noTargets(Card* caster, const Event* e) const
{
    return m_group.isEmpty(caster, e);
}

Owner TargetsFromGroupExpression::owner() const
{
    return m_group.owner();
}

int TargetsFromGroupExpression::howManySelections() const
{
    return m_quantity;
}

PickingTypes::PickingType TargetsFromGroupExpression::pickingType() const
{
    return m_type;
}

bool TargetsFromGroupExpression::withReplacement() const
{
    return m_withReplacement;
}



QString TargetsFromGroupExpression::toString() const
{
    if (m_type == PickingTypes::RANDOM) {
        if (m_withReplacement)
            return QString("(Pick %1 random target(s) with replacement in ").arg(m_quantity) + m_group.toString() + ")";
        else
            return QString("(Pick %1 random target(s) without replacement in ").arg(m_quantity) + m_group.toString() + ")";
    }
    else if (m_type == PickingTypes::USER_SELECT) {
        return QString("(Select a target in ") + m_group.toString() + ")";
    }
    else
        return QString("(Select all targets in ") + m_group.toString() + ")";
}

QVector<Character*>* TargetsFromGroupExpression::eval(Card* caster, Action* forAction, const Event* e)
{
    if (m_type == PickingTypes::ALL)
        return m_group.eval(caster, e);

    QVector<Character*>* targets = new QVector<Character*>();
    QVector<Character*>* group = m_group.eval(caster, e);
    if (m_type == PickingTypes::USER_SELECT) {
        targets->append(caster->owner()->selectTarget(group, forAction));
        delete group;
        return targets;
    }

    // else m_type == RANDOM
    for (int i = 0; i < m_quantity; i += 1) {
        if (group->empty())
            break;

        if (m_withReplacement)
            targets->append(group->at((qrand() % group->size())));
        else
            targets->append(group->takeAt((qrand() % group->size())));
    }
    delete group;

    return targets;
}

bool TargetsFromGroupExpression::selectionRequired() const
{
    return m_type == PickingTypes::USER_SELECT;
}


bool TargetsFromGroupExpression::canBeHero() const
{
    return m_group.acceptedSubtypes() & CharacterTypes::HERO;
}
