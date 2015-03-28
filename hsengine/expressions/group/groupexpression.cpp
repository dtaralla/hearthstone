#include "groupexpression.h"
#include "card.h"
#include "expressions/target/targetexpression.h"
#include "player.h"
#include "game.h"

GroupExpression::GroupExpression(Owner fromPlayer, CharacterType type, const QSharedPointer<TargetExpression>& excludedTargets) :
    m_owner(fromPlayer),
    m_type(type),
    m_excludedTargets(excludedTargets)
{

}

GroupExpression::~GroupExpression()
{
    m_excludedTargets.clear();
}

QVector<Character*>* GroupExpression::eval(Card* caster, const Event* e) const
{
    QVector<Character*>* group = caster->owner()->game()->charactersByPlayTime(caster->owner(), m_owner, m_type);

    if (!m_excludedTargets.isNull()) {
        QVector<Character*>* excluded = m_excludedTargets.data()->eval(caster, NULL, e);
        foreach (Character* c, *excluded)
            group->removeOne(c);
        delete excluded;
    }

    return group;
}

bool GroupExpression::isEmpty(Card* caster, const Event* e) const
{
    QVector<Character*>* group = eval(caster, e);
    bool empty = group->empty();
    delete group;
    return empty;
}

CharacterType GroupExpression::acceptedSubtypes() const
{
    return m_type;
}

Owner GroupExpression::owner() const
{
    return m_owner;
}



QString GroupExpression::toString() const
{
    return "(all characters with subtype " +
            CharacterTypes::toString(m_type) + " of " + Owners::toString(m_owner) +
            (m_excludedTargets.isNull() ? QString(")") : (", without " + m_excludedTargets.data()->toString() + ")"));
}
