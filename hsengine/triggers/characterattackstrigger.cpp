#include "characterattackstrigger.h"
#include "event.h"
#include "character.h"
#include "expressions/target/targetexpression.h"

CharacterAttacksTrigger::CharacterAttacksTrigger(QVector<Action*>* actions,
                                                 const QSharedPointer<TargetExpression>& attackers,
                                                 const QSharedPointer<TargetExpression>& attackedCharacters) :
    Trigger(actions),
    mSources(attackers),
    mTargets(attackedCharacters)
{
}

CharacterAttacksTrigger::CharacterAttacksTrigger(Action* action,
                                                 const QSharedPointer<TargetExpression>& attackers,
                                                 const QSharedPointer<TargetExpression>& attackedCharacters) :
    Trigger(action),
    mSources(attackers),
    mTargets(attackedCharacters)
{
}

CharacterAttacksTrigger::~CharacterAttacksTrigger()
{
    mSources.clear();
    mTargets.clear();
}



bool CharacterAttacksTrigger::listensTo(const Event& e) const
{
    // Apply targets filter if any
    if (!mTargets.isNull()) {
        Character* atkedChar = VPtr<Character>::AsPtr(e.extra("attackedCharacter"));
        QVector<Character*>* group = mTargets->eval(m_card, NULL, &e);
        if (!group->contains(atkedChar)) {
            delete group;
            return false;
        }
        delete group;
    }
    // Arrived here -> passed the "targets" filter!

    // Apply sources filter if any
    if (!mSources.isNull()) {
        Character* atker = VPtr<Character>::AsPtr(e.extra("attackingCharacter"));
        QVector<Character*>* group = mSources.data()->eval(m_card, NULL, &e);
        if (!group->contains((Character*) atker)) {
            delete group;
            return false;
        }
        delete group;
    }

    return true;
}

Trigger* CharacterAttacksTrigger::clone() const
{
    return new CharacterAttacksTrigger(m_actions, mSources, mTargets);
}
