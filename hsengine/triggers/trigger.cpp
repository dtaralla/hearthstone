#include "trigger.h"
#include "event.h"
#include "actions/action.h"

Trigger::Trigger(QVector<Action*>* actions) :
    m_card(NULL),
    m_actions(actions)
{

}

Trigger::Trigger(Action* action) :
    m_card(NULL),
    m_actions(new QVector<Action*>(1, action))
{

}

Trigger::~Trigger()
{
    qDeleteAll(*m_actions);
    delete m_actions;
}

QVector<Action*>* Trigger::actions() const
{
    return m_actions;
}

Trigger* Trigger::setSourceCard(Card* c) const
{
    Trigger* toRet = clone();
    toRet->m_card = c;
    QVector<Action*>* clonedActions = new QVector<Action*>();
    foreach (Action* a, *m_actions)
        clonedActions->append(a->setSourceCard(c));
    toRet->m_actions = clonedActions;

    return toRet;
}

