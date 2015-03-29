#include "characterdamagedtrigger.h"
#include "event.h"
#include "character.h"
#include "expressions/target/targetexpression.h"

CharacterDamagedTrigger::CharacterDamagedTrigger(QVector<Action*>* actions,
                                                 const QSharedPointer<TargetExpression>& damagers,
                                                 const QSharedPointer<TargetExpression>& damagedCharacters,
                                                 SourceType accepedSources) :
    Trigger(actions),
    m_damagers(damagers),
    m_damagedCharacters(damagedCharacters),
    m_acceptedSources(accepedSources)
{
    if (m_acceptedSources == ANY || m_acceptedSources == SPELL) {
        m_damagers.clear();
        m_damagers = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterDamagedTrigger::CharacterDamagedTrigger(Action* action,
                                                 const QSharedPointer<TargetExpression>& damagers,
                                                 const QSharedPointer<TargetExpression>& damagedCharacters,
                                                 SourceType accepedSources) :
    Trigger(action),
    m_damagers(damagers),
    m_damagedCharacters(damagedCharacters),
    m_acceptedSources(accepedSources)
{
    if (m_acceptedSources == ANY || m_acceptedSources == SPELL) {
        m_damagers.clear();
        m_damagers = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterDamagedTrigger::~CharacterDamagedTrigger()
{
    m_damagers.clear();
    m_damagedCharacters.clear();
}



bool CharacterDamagedTrigger::listensTo(const Event& e) const
{
    // Apply damagedCharacters filter if any
    if (!m_damagedCharacters.isNull()) {
        Character* damagedChar = VPtr<Character>::AsPtr(e.extra("damagedCharacter"));
        QVector<Character*>* group = m_damagedCharacters->eval(m_card, NULL, &e);
        if (!group->contains(damagedChar)) {
            delete group;
            return false;
        }
        delete group;
    }
    // Arrived here -> passed the "damagedCharacters" filter!

    if (m_acceptedSources == ANY) {
        return true;
    }
    // Arrived here -> Special damage source! Need to filter damager

    Card* damager = VPtr<Card>::AsPtr(e.extra("damager"));
    if (damager == NULL)
        return false; // Damager can't be a fatigue card if acceptedSources != ANY

    if (m_acceptedSources == SPELL)
        return damager->type() == CardTypes::CARD_SPELL;

    // Arrived here: the accepted source is Characters (only)

    // Apply damagers filter if any
    if (!m_damagers.isNull()) {
        switch (m_acceptedSources) {
            case CHARACTER:
                if (!(damager->type() & CardTypes::CARD_CHARACTER))
                    return false;
                break;

            case HERO:
                if (damager->type() != CardTypes::CARD_HERO)
                    return false;
                break;

            case MINION:
                if (damager->type() != CardTypes::CARD_MINION)
                    return false;
                break;

            default:
                qCritical() << "Unrecognized accepted source" << m_acceptedSources;
        }

        QVector<Character*>* group = m_damagers.data()->eval(m_card, NULL, &e);
        if (!group->contains((Character*) damager)) {
            delete group;
            return false;
        }
        delete group;
    }

    return true;
}

Trigger* CharacterDamagedTrigger::clone() const
{
    return new CharacterDamagedTrigger(m_actions, m_damagers, m_damagedCharacters, m_acceptedSources);
}
