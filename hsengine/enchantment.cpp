#include "enchantment.h"
#include "actions/destroyenchantmentaction.h"
#include "card.h"
#include "spell.h"
#include "character.h"
#include "triggers/turnendtrigger.h"

Enchantment::Enchantment(bool temporary, const QString& name, const QString& desc,
                         Ability abilities,
                         const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
                         int maxHPModifier, int atkModifier, int costModifier,
                         int spellDmgModifier) :
    m_source(NULL),
    m_buffedCard(NULL),
    m_temporary(temporary),
    m_name(name),
    m_desc(desc),
    m_abilities(abilities),
    m_triggerPowers(triggerPowers),
    m_maxHPModifier(maxHPModifier),
    m_atkModifier(atkModifier),
    m_costModifier(costModifier),
    m_spellDmgModifier(spellDmgModifier),
    m_specialized(false)
{
}

Enchantment::Enchantment(const Enchantment& e) :
    m_source(NULL),
    m_buffedCard(NULL),
    m_temporary(e.m_temporary),
    m_name(e.m_name),
    m_desc(e.m_desc),
    m_abilities(e.m_abilities),
    m_triggerPowers(e.m_triggerPowers),
    m_maxHPModifier(e.m_maxHPModifier),
    m_atkModifier(e.m_atkModifier),
    m_costModifier(e.m_costModifier),
    m_spellDmgModifier(e.m_spellDmgModifier),
    m_specialized(false)
{
}

Enchantment::~Enchantment()
{
    foreach (QVector<Trigger*>* v, m_triggerPowers) {
        qDeleteAll(*v);
        delete v;
    }
}

QString Enchantment::name() const
{
    return m_name;
}

Card* Enchantment::sourceCard() const
{
    return m_source;
}

Card* Enchantment::buffedCard() const
{
    return m_buffedCard;
}

Ability Enchantment::abilities() const
{
    return m_abilities;
}

const QHash<Event::Type, QVector<Trigger*>*>* Enchantment::triggerPowers() const
{
    return &m_triggerPowers;
}

int Enchantment::maxHPModifier() const
{
    return m_maxHPModifier;
}

int Enchantment::atkModifier() const
{
    return m_atkModifier;
}

int Enchantment::costModifier() const
{
    return m_costModifier;
}

void Enchantment::apply()
{
    Q_ASSERT(m_specialized);

    // Apply mana modifiers
    m_buffedCard->addCostModifier(m_costModifier);

    // Apply additional trigger powers
    m_buffedCard->addTriggerPowers(m_triggerPowers);

    // Apply spell dmg modifiers
    if (m_spellDmgModifier > 0) {
        // Constructor ensures this is therefore applied on a Spell: cast is safe
        ((Spell*) m_buffedCard)->addDmgModifier(m_spellDmgModifier);
    }

    // Apply character modifiers
    if (m_atkModifier > 0 || m_maxHPModifier > 0 || m_abilities != Abilities::NO_ABILITY) {
        // Constructor ensures this is therefore applied on a minion: cast is safe
        Character* buffedChar = (Character*) m_buffedCard;
        buffedChar->addAtkModifier(m_atkModifier);
        buffedChar->addMaxHPModifier(m_maxHPModifier);
        buffedChar->addAbilities(m_abilities);
    }
}

Enchantment* Enchantment::specialize(Card* sourceCard, Card* buffedCard)
{
    Q_ASSERT(m_specialized == false); // Enchantments are single-target,
                                      // single-source.
    Enchantment* toRet = new Enchantment(*this);

    toRet->m_source = sourceCard;
    toRet->m_buffedCard = buffedCard;

    // Some modifiers only apply to a certain card types.
    if (m_spellDmgModifier > 0)
        Q_ASSERT(buffedCard->type() == CardTypes::CARD_SPELL);
    if (m_maxHPModifier > 0 || m_atkModifier > 0 || m_abilities != Abilities::NO_ABILITY)
        Q_ASSERT(buffedCard->type() & CardTypes::CARD_CHARACTER);


    // Specialize each (event;triggers) for the buffed card
    foreach (Event::Type key, m_triggerPowers.keys()) {
        QVector<Trigger*>* newPowers = new QVector<Trigger*>();
        foreach (Trigger* t, *m_triggerPowers.value(key))
            newPowers->append(t->setSourceCard(buffedCard));
        toRet->m_triggerPowers.insert(key, newPowers);
    }

    // Register a "delete this enchantment" action for end of this turn if temp
    if (m_temporary) {
        Action* a = new DestroyEnchantmentAction(toRet);
        TurnEndTrigger* t = new TurnEndTrigger(a);
        Trigger* t2 = t->setSourceCard(buffedCard);
        delete t;

        if (toRet->m_triggerPowers.contains(Event::AT_TURN_END)) {
            toRet->m_triggerPowers.value(Event::AT_TURN_END)->append(t2);
        }
        else {
            toRet->m_triggerPowers.insert(Event::AT_TURN_END, new QVector<Trigger*>(1, t2));
        }
    }

    toRet->m_specialized = true;
    return toRet;
}



QString Enchantment::toString() const
{
    return m_desc;
}
