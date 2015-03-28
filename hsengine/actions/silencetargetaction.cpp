#include "silencetargetaction.h"
#include "card.h"
#include "player.h"
#include "character.h"
#include "silenceenchantment.h"
#include "iorequest.h"

SilenceTargetAction::SilenceTargetAction(int id, const QSharedPointer<TargetExpression>& target) :
    TargetedAction(id, target)
{
}

SilenceTargetAction::~SilenceTargetAction()
{

}



QString SilenceTargetAction::toString() const
{
    return "Silence a minion";
}

ActionType SilenceTargetAction::type() const
{
    return ActionTypes::SILENCE_TARGET;
}

void SilenceTargetAction::resolve(const Event* e)
{
    Q_CHECK_PTR(m_card);

    QVector<Character*>* targets = m_targets(m_card, e);
    foreach (Character* c, *targets) {
        Q_ASSERT(c->type() == CardTypes::CARD_MINION);
        Enchantment* se = new SilenceEnchantment(m_card, c);
        c->addEnchantment(se);
    }

    m_refreshLastTargets(targets);
    delete targets;
}

SilenceTargetAction* SilenceTargetAction::cloneFlyweight() const
{
    return new SilenceTargetAction(m_id, m_target);
}
