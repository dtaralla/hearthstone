#include "characterdamagedtrigger.h"
#include "event.h"
#include "character.h"
#include "expressions/target/targetexpression.h"

CharacterDamagedTrigger::CharacterDamagedTrigger(QVector<Action*>* actions,
                                                 const QSharedPointer<TargetExpression>& damagers,
                                                 const QSharedPointer<TargetExpression>& damagedCharacters,
                                                 SourceType accepedSources) :
    Trigger(actions),
    mSources(damagers),
    mTargets(damagedCharacters),
    mAcceptedSourceType(accepedSources)
{
    if (mAcceptedSourceType == ANY || mAcceptedSourceType == SPELL) {
        mSources.clear();
        mSources = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterDamagedTrigger::CharacterDamagedTrigger(Action* action,
                                                 const QSharedPointer<TargetExpression>& damagers,
                                                 const QSharedPointer<TargetExpression>& damagedCharacters,
                                                 SourceType accepedSources) :
    Trigger(action),
    mSources(damagers),
    mTargets(damagedCharacters),
    mAcceptedSourceType(accepedSources)
{
    if (mAcceptedSourceType == ANY || mAcceptedSourceType == SPELL) {
        mSources.clear();
        mSources = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterDamagedTrigger::~CharacterDamagedTrigger()
{
    mSources.clear();
    mTargets.clear();
}



bool CharacterDamagedTrigger::listensTo(const Event& e) const
{
    // Apply targets filter if any
    if (!mTargets.isNull()) {
        Character* damagedChar = VPtr<Character>::AsPtr(e.extra("damagedCharacter"));
        QVector<Character*>* group = mTargets->eval(m_card, NULL, &e);
        if (!group->contains(damagedChar)) {
            delete group;
            return false;
        }
        delete group;
    }
    // Arrived here -> passed the "targets" filter!

    if (mAcceptedSourceType == ANY) {
        return true;
    }
    // Arrived here -> Special damage source! Need to filter source

    Card* damager = VPtr<Card>::AsPtr(e.extra("damager"));
    if (damager == NULL)
        return false; // Damager can't be a fatigue card if acceptedSources != ANY

    if (mAcceptedSourceType == SPELL)
        return damager->type() == CardTypes::CARD_SPELL;

    // Arrived here: the accepted source is Characters (only)

    // Apply damagers filter if any
    if (!mSources.isNull()) {
        switch (mAcceptedSourceType) {
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
                qCritical() << "Unrecognized accepted source" << mAcceptedSourceType;
        }

        QVector<Character*>* group = mSources.data()->eval(m_card, NULL, &e);
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
    return new CharacterDamagedTrigger(m_actions, mSources, mTargets, mAcceptedSourceType);
}
