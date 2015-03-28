#include "eventcharacterparamexpression.h"
#include "event.h"
#include "card.h"
#include <QtDebug>

EventCharacterParamExpression::EventCharacterParamExpression(Event::Type type, const QVector<EventParam>& toGet) :
    m_type(type),
    m_toGet(toGet)
{
    // Prevent non-character event params
    Q_ASSERT(m_type != Event::AFTER_CARD_PLAYED &&
             m_type != Event::AT_TURN_END &&
             m_type != Event::BEFORE_CARD_PLAYED &&
             m_type != Event::NO_EVENT);
}

EventCharacterParamExpression::~EventCharacterParamExpression()
{

}



QString EventCharacterParamExpression::toString() const
{
    QString s = "(";
    foreach (EventParam ep, m_toGet)
        s += QString(ep) + ", ";

    return s.left(s.size() - 2) + ")";
}

QVector<Character*>* EventCharacterParamExpression::eval(Card* caster, Action* forAction, const Event* e)
{
    Q_UNUSED(forAction);
    QVector<Character*>* toRet = new QVector<Character*>();
    if (e == NULL) {
        qWarning() << *caster << "uses (EventCharacterParam) target expression without any event";
        return toRet;
    }

    // Perform safety checks when needed by certain types
    switch (e->type()) {
        case Event::CHARACTER_DAMAGED:
            if (m_toGet.contains(EventParams::DAMAGER)) {
                Card* damager = VPtr<Card>::AsPtr(e->extra(EventParams::DAMAGER));
                if (damager == NULL || !(damager->type() & CardTypes::CARD_CHARACTER)) {
                    qWarning() << "Event '" << *e << "' damager is not a character!";
                    return toRet;
                }
            }
            break;

        case Event::CHARACTER_HEALED:
            if (m_toGet.contains(EventParams::HEALER)) {
                Card* healer = VPtr<Card>::AsPtr(e->extra(EventParams::HEALER));
                if (healer == NULL || !(healer->type() & CardTypes::CARD_CHARACTER)) {
                    qWarning() << "Event '" << *e << "' healer is not a character!";
                    return toRet;
                }
            }
            break;

        default:
            ;
    }

    // If a param is incompatible with the event type, the program will abort
    foreach (EventParam ep, m_toGet) {
        toRet->append(VPtr<Character>::AsPtr(e->extra(ep)));
    }

    return toRet;
}

bool EventCharacterParamExpression::noTargets(Card* caster, const Event* e) const
{
    Q_UNUSED(caster);
    return e == NULL || (e->type() != Event::CHARACTER_ATTACKS &&
                         e->type() != Event::CHARACTER_DAMAGED &&
                         e->type() != Event::CHARACTER_HEALED &&
                         e->type() != Event::MINION_DIES);
}

bool EventCharacterParamExpression::selectionRequired() const
{
    return false;
}
