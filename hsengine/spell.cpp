#include "spell.h"
#include "spellinfo.h"
#include "actions/action.h"
#include "actions/playaction.h"

Spell::Spell(const CardIdentity* baseSpell, Player* owner) :
    Card(owner),
    m_playAction(new PlayAction(this)),
    m_spell(NULL),
    m_dmgModifier(0)
{
    Q_ASSERT(baseSpell->type() == CardTypes::CARD_SPELL);
    m_base = (const SpellInfo*) baseSpell;
}

Spell::~Spell()
{
    delete m_playAction;
    if (m_spell != NULL) {
        qDeleteAll(*m_spell);
        delete m_spell;
    }
}



PlayAction* Spell::playAction()
{
    return m_playAction;
}

const QVector<Action*>* Spell::playActionConsequences() const
{
    Q_CHECK_PTR(m_spell);
    return m_spell;
}

const CardIdentity* Spell::base() const
{
    return m_base;
}

CardType Spell::type() const
{
    return CardTypes::CARD_SPELL;
}

void Spell::initCard(Player* owner)
{
    Card::initCard(owner);
    Q_ASSERT(m_spell == NULL);

    m_spell = new QVector<Action*>();
    foreach (Action* a, *m_base->spell()) {
        m_spell->append(a->setSourceCard(this));
    }
}

void Spell::addDmgModifier(int mod)
{
    m_dmgModifier += mod;
    foreach (Action* a, *m_spell) {
        a->addSpellDmg(mod);
    }
}

int Spell::dmgModifier() const
{
    return m_dmgModifier;
}

void Spell::clearEnchantmentEffects()
{
    Card::clearEnchantmentEffects();
    foreach (Action* a, *m_spell) {
        a->addSpellDmg(-m_dmgModifier);
    }
    m_dmgModifier = 0;
}
