#include "silenceenchantment.h"
#include "card.h"
#include "character.h"
#include "triggers/trigger.h"

SilenceEnchantment::SilenceEnchantment(Card* sourceCard, Card* buffedCard) :
    Enchantment(false, "Silence", "", Abilities::NO_ABILITY,
                QHash<Event::Type, QVector<Trigger*>*>(), 0, 0, 0, 0),
    m_applied(false)
{
    m_source = sourceCard;
    m_buffedCard = buffedCard;
    m_specialized = true;
    Q_ASSERT(buffedCard->type() == CardTypes::CARD_MINION);
}

SilenceEnchantment::~SilenceEnchantment()
{
    qDeleteAll(m_silencedEnchantments);
}


Ability SilenceEnchantment::abilities() const
{
    return Abilities::NO_ABILITY;
}

const QHash<Event::Type, QVector<Trigger*>*>* SilenceEnchantment::triggerPowers() const
{
    return NULL;
}

int SilenceEnchantment::maxHPModifier() const
{
    return 0;
}

int SilenceEnchantment::atkModifier() const
{
    return 0;
}

int SilenceEnchantment::costModifier() const
{
    return 0;
}

void SilenceEnchantment::apply()
{
    // Apply a silence only once
    if (!m_applied) {
        QVector<Enchantment*>* silenced = m_buffedCard->takeEnchantmentsBefore(this);
        m_silencedEnchantments << *silenced;
        delete silenced;
        m_applied = true;
        if (m_buffedCard->type() & CardTypes::CARD_CHARACTER)
            ((Character*) m_buffedCard)->setSilenced(true);

        m_buffedCard->reApplyEnchantmentEffects();
    }
}

QString SilenceEnchantment::toString() const
{
    QString toRet("<ul><li>Base abilities (if any)</li>");
    foreach (Enchantment* e, m_silencedEnchantments) {
        toRet += "<li><b>" + e->name() + "</b>:" + e->toString() + "</li>";
    }

    return toRet + "</ul>";
}
