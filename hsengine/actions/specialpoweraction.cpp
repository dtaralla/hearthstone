#include "specialpoweraction.h"
#include "card.h"
#include "player.h"
#include "hero.h"
#include "dbouput.h"
#include "game.h"

SpecialPowerAction::SpecialPowerAction(int id, const QString& name, const QString& desc,
                                       int manaCost, QVector<Action*>* effects) :
    Action(),
    m_id(id),
    m_name(name),
    m_desc(desc),
    m_cost(manaCost),
    m_effects(effects),
    m_alreadyUsed(false)
{

}

SpecialPowerAction::~SpecialPowerAction()
{
    qDeleteAll(*m_effects);
    delete m_effects;
}

ActionType SpecialPowerAction::type() const
{
    return ActionTypes::SPECIAL_POWER;
}



void SpecialPowerAction::resolve(const Event* e)
{
    Q_UNUSED(e);
    Q_CHECK_PTR(m_card);
    Game* game = m_card->owner()->game();
    m_card->owner()->modifyCurrentMana(-m_cost);
    m_alreadyUsed = true;

    foreach (Action* a, *m_effects)
        if (Game::IsDBGenerationModeOn() && a->isTargetedAction()) {
            DBOutput::Instance(game, DBOutput::BOARD_CONTROL)->buffer(game->environment(), a);
            DBOutput::Instance(game, DBOutput::AGGRO)->buffer(game->environment(), a);

            Game::AggroScore* aScore1 = game->meta_AggroScore();

            a->resolve();

            Game::AggroScore* aScore2 = game->meta_AggroScore();
            float aScore = aScore2->score - aScore1->score;
            delete aScore1; aScore1 = NULL;
            delete aScore2; aScore2 = NULL;

            DBOutput::Instance(game, DBOutput::AGGRO)->buffer(aScore, a);

            Game::BoardControlScore* s = game->meta_BoardControlScore();
            DBOutput::Instance(game, DBOutput::BOARD_CONTROL)->buffer(s->score, a);
            delete s;
        }
        else
            a->resolve();
}

Action* SpecialPowerAction::cloneFlyweight() const
{
    return new SpecialPowerAction(m_id, m_name, m_desc, m_cost, m_effects);
}


bool SpecialPowerAction::resolvable(const Event* e) const
{
    if (m_alreadyUsed || m_card->owner()->currentMana() < m_cost)
        return false;

    foreach (Action* a, *m_effects)
        if (!a->resolvable(e))
            return false;

    return true;
}


Action* SpecialPowerAction::setSourceCard(Card* c) const
{
    Action* toRet = Action::setSourceCard(c);

    QVector<Action*>* specializedEffects = new QVector<Action*>();
    foreach (Action* a, *m_effects) {
        specializedEffects->append(a->setSourceCard(c));
    }
    ((SpecialPowerAction*) toRet)->m_effects = specializedEffects;

    return toRet;
}

const QString& SpecialPowerAction::name() const
{
    return m_name;
}

const QString& SpecialPowerAction::desc() const
{
    return m_desc;
}

int SpecialPowerAction::cost() const
{
    return m_cost;
}

void SpecialPowerAction::setAlreadyUsed(bool used)
{
    m_alreadyUsed = used;
}

bool SpecialPowerAction::alreadyUsed() const
{
    return m_alreadyUsed;
}

const QVector<Action*>*SpecialPowerAction::effects() const
{
    return m_effects;
}

int SpecialPowerAction::id() const
{
    return m_id;
}

QString SpecialPowerAction::toString() const
{
    return "Play " + m_name;
}
