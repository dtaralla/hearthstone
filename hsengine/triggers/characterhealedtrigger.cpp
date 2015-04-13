#include "characterhealedtrigger.h"
#include "event.h"
#include "character.h"
#include "expressions/target/targetexpression.h"

CharacterHealedTrigger::CharacterHealedTrigger(QVector<Action*>* actions,
                                                 const QSharedPointer<TargetExpression>& healers,
                                                 const QSharedPointer<TargetExpression>& healedCharacters,
                                                 SourceType accepedSources) :
    Trigger(actions),
    mSources(healers),
    mTargets(healedCharacters),
    mAcceptedSourceType(accepedSources)
{
    if (mAcceptedSourceType == ANY || mAcceptedSourceType == SPELL) {
        mSources.clear();
        mSources = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterHealedTrigger::CharacterHealedTrigger(Action* action,
                                                 const QSharedPointer<TargetExpression>& healers,
                                                 const QSharedPointer<TargetExpression>& healedCharacters,
                                                 SourceType accepedSources) :
    Trigger(action),
    mSources(healers),
    mTargets(healedCharacters),
    mAcceptedSourceType(accepedSources)
{
    if (mAcceptedSourceType == ANY || mAcceptedSourceType == SPELL) {
        mSources.clear();
        mSources = QSharedPointer<TargetExpression>(NULL);
    }
}

CharacterHealedTrigger::~CharacterHealedTrigger()
{
    mSources.clear();
    mTargets.clear();
}



bool CharacterHealedTrigger::listensTo(const Event& e) const
{
    // Apply targets filter if any
    if (!mTargets.isNull()) {
        Character* healedChar = VPtr<Character>::AsPtr(e.extra("healedCharacter"));
        QVector<Character*>* group = mTargets->eval(m_card, NULL, &e);
        if (!group->contains(healedChar)) {
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

    Card* healer = VPtr<Card>::AsPtr(e.extra("damager"));
    if (healer == NULL)
        return false; // Damager can't be a fatigue card if acceptedSources != ANY

    if (mAcceptedSourceType == SPELL)
        return healer->type() == CardTypes::CARD_SPELL;

    // Arrived here: the accepted source is Characters (only)

    // Apply healers filter if any
    if (!mSources.isNull()) {
        switch (mAcceptedSourceType) {
            case CHARACTER:
                if (!(healer->type() & CardTypes::CARD_CHARACTER))
                    return false;
                break;

            case HERO:
                if (healer->type() != CardTypes::CARD_HERO)
                    return false;
                break;

            case MINION:
                if (healer->type() != CardTypes::CARD_MINION)
                    return false;
                break;

            default:
                qCritical() << "Unrecognized accepted source" << mAcceptedSourceType;
        }

        QVector<Character*>* group = mSources.data()->eval(m_card, NULL, &e);
        if (!group->contains((Character*) healer)) {
            delete group;
            return false;
        }
        delete group;
    }

    return true;
}

Trigger* CharacterHealedTrigger::clone() const
{
    return new CharacterHealedTrigger(m_actions, mSources, mTargets, mAcceptedSourceType);
}
