#include "summonnewminionsaction.h"
#include "carddb.h"
#include "characterinfo.h"
#include "minion.h"
#include "player.h"
#include "game.h"

SummonNewMinionsAction::SummonNewMinionsAction(int id, int quantity, Owner targetOwner) :
    Action(),
    m_quantity(quantity),
    m_targetOwner(targetOwner)
{
    const CardIdentity* ci = CardDB::Instance()->cardInfo(id);
    Q_ASSERT(ci->type() == CardTypes::CARD_MINION);
    m_base = (const CharacterInfo*) ci;
}

SummonNewMinionsAction::~SummonNewMinionsAction()
{

}

const CharacterInfo* SummonNewMinionsAction::base() const
{
    return m_base;
}

int SummonNewMinionsAction::quantity() const
{
    return m_quantity;
}

Owner SummonNewMinionsAction::targetOwner() const
{
    return m_targetOwner;
}



QString SummonNewMinionsAction::toString() const
{
    return QString("Summon %1 ").arg(m_quantity) + m_base->name() + " for " +
            Owners::toString(m_targetOwner);
}

ActionType SummonNewMinionsAction::type() const
{
    return ActionTypes::SUMMON_MINIONS;
}

void SummonNewMinionsAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    Q_CHECK_PTR(m_card);
    Game* game = m_card->owner()->game();

    if (m_targetOwner & Owners::ALLY) {
        Player* owner = m_card->owner();
        for (int i = 0; i < m_quantity; i += 1) {
            if (owner->minions()->size() >= 7)
                break;

            Minion* m = new Minion(m_base);
            m->initCard(owner);
            game->summonMinion(m, -1);
        }
    }

    if (m_targetOwner & Owners::ENEMY) {
        Player* owner = m_card->owner()->opponent();
        for (int i = 0; i < m_quantity; i += 1) {
            if (owner->minions()->size() >= 7)
                break;

            Minion* m = new Minion(m_base);
            m->initCard(owner);
            game->summonMinion(m, -1);
        }
    }
}

Action* SummonNewMinionsAction::cloneFlyweight() const
{
    return new SummonNewMinionsAction(m_base->id(), m_quantity, m_targetOwner);
}

