#include "minion.h"
#include "actions/summonminionaction.h"
#include "carddb.h"
#include "actions/playaction.h"

Minion::Minion(const CharacterIdentity* baseChar, Player* owner) :
    Character(baseChar, owner),
    m_playActionConsequences(new QVector<Action*>()),
    m_playAction(new PlayAction(this)),
    m_battlecryResolved(false)
{
    m_playActionConsequences->append(new SummonMinionAction(this));
}

Minion::~Minion()
{
    qDeleteAll(m_battlecry);
    qDeleteAll(m_deathRattles);
    delete m_playAction;
    qDeleteAll(*m_playActionConsequences);
    delete m_playActionConsequences;
}

PlayAction* Minion::playAction()
{
    return m_playAction;
}

const QVector<Action*>* Minion::playActionConsequences() const
{
    return m_playActionConsequences;
}

CardType Minion::type() const
{
    return CardTypes::CARD_MINION;
}

CharacterType Minion::subtype() const
{
    return m_base->subtype();
}

QVector<Action*>* Minion::deathRattles() const
{
    if (m_silenced)
        return new QVector<Action*>();

    return new QVector<Action*>(m_deathRattles);
}

const QVector<Action*>* Minion::battlecry() const
{
    return &m_battlecry;
}


void Minion::initCard(Player* cardOwner)
{
    Character::initCard(cardOwner);

    // Clone deathrattles making them linked to this very card
    foreach (Action* a, *m_base->deathrattle())
        m_deathRattles << a->setSourceCard(this);

    // Clone battlecry making its actions linked to this very card
    foreach (Action* a, *m_base->battlecry())
        m_battlecry << a->setSourceCard(this);
}

void Minion::setBattlecryResolved(bool resolved)
{
    m_battlecryResolved = resolved;
}

bool Minion::battlecryResolved() const
{
    return m_battlecryResolved;
}
