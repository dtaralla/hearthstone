#include "targettedaction.h"
#include "expressions/target/targetexpression.h"
#include "character.h"

TargetedAction::TargetedAction(int id, const QSharedPointer<TargetExpression>& target) :
    Action(),
    m_id(id),
    m_target(target)
{

}

TargetedAction::~TargetedAction()
{
    m_target.clear();
}

const TargetExpression* TargetedAction::target() const
{
    return m_target.data();
}

const QVector<Character*>* TargetedAction::lastTargets() const
{
    return &m_lastTargets;
}

void TargetedAction::forceTargets(const QVector<Character*>& targets)
{
    m_forcedTargets = QVector<Character*>(targets);
}

int TargetedAction::id() const
{
    return m_id;
}

QVector<Character*>* TargetedAction::m_targets(Card* caster, const Event* e)
{
    if (m_forcedTargets.empty())
        return m_target->eval(caster, this, e);

    return new QVector<Character*>(m_forcedTargets);
}

void TargetedAction::m_refreshLastTargets(const QVector<Character*>* lastTargets)
{
    m_lastTargets = QVector<Character*>(*lastTargets);

    QMutableVectorIterator<Character*> i(m_lastTargets);
    while (i.hasNext()) {
        Character* c = i.next();
        if (c->dying())
            i.remove();
    }
}

bool TargetedAction::resolvable(const Event* e) const
{
    if (!m_forcedTargets.empty())
        return true;

    if (m_target->selectionRequired() && m_target->noTargets(m_card, e))
        return false;

    return true;
}

bool TargetedAction::isTargetedAction() const
{
    return m_target->selectionRequired();
}

