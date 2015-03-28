#include "card.h"
#include "cardinfo.h"
#include "actions/action.h"
#include "triggers/trigger.h"
#include "enchantment.h"
#include "iorequest.h"
#include "player.h"
#include "minion.h"

Card::Card(Player* owner) :
    m_owner(owner),
    m_costModifier(0)
{
}

Card::~Card()
{
    foreach (QVector<Trigger*>* va, m_triggerPowers.values()) {
        qDeleteAll(*va);
        delete va;
    }

    foreach (Enchantment* e, m_enchantments)
        delete e;
    clearEnchantmentEffects();
}

const QString& Card::name() const
{
    return base()->name();
}

Player* Card::owner() const
{
    return m_owner;
}

void Card::initCard(Player* cardOwner)
{
    // Init owner
    m_owner = cardOwner;

    Q_ASSERT(m_triggerPowers.empty()); // init should not be called more than once

    // Clone actions making them linked to this very card
    foreach (Event::Type e, base()->triggerPowers().keys()) {
        QVector<Trigger*>* triggers = new QVector<Trigger*>();
        foreach (Trigger* t, *base()->triggerPowers().value(e))
            triggers->append(t->setSourceCard(this));
        m_triggerPowers.insert(e, triggers);
    }
}

int Card::effectiveManaCost() const
{
    return base()->manaCost() + m_costModifier;
}

bool Card::listensTo(const Event& e) const
{
    if (m_triggerPowers.contains(e.type())) {
        foreach (Trigger* t, *m_triggerPowers.value(e.type())) {
            if (t->listensTo(e))
                return true;
        }
    }

    if (m_additionalTriggerPowers.contains(e.type())) {
        foreach (Trigger* t, *m_additionalTriggerPowers.value(e.type())) {
            if (t->listensTo(e))
                return true;
        }
    }

    return false;
}

QVector<Action*>* Card::actionsOnEvent(const Event& e) const
{
    //Q_ASSERT(listensTo(e));

    QVector<Action*>* as = new QVector<Action*>();
    if (m_triggerPowers.contains(e.type()))
        foreach (Trigger* t, *m_triggerPowers.value(e.type()))
            if (t->listensTo(e))
                *as << *t->actions();

    if (m_additionalTriggerPowers.contains(e.type()))
        foreach (Trigger* t, *m_additionalTriggerPowers.value(e.type()))
            if (t->listensTo(e))
                *as << *t->actions();

    Q_ASSERT(!as->empty());
    return as;
}

int Card::triggerPowersCount() const
{
    return m_triggerPowers.size() + m_additionalTriggerPowers.size();
}

void Card::addCostModifier(int mod)
{
    m_costModifier += mod;
}

void Card::addTriggerPowers(const QHash<Event::Type, QVector<Trigger*>*>& tps)
{
    foreach (Event::Type t, tps.keys()) {
        if (m_additionalTriggerPowers.contains(t))
            *m_additionalTriggerPowers.value(t) << *tps.value(t);
        else {
            QVector<Trigger*>* ts = new QVector<Trigger*>(*tps.value(t));
            m_additionalTriggerPowers.insert(t, ts);
        }
    }
}

QString Card::toString() const
{
    return name() + " (" + m_owner->name() + ")";
}


void Card::addEnchantment(Enchantment* e)
{
    m_enchantments.append(e);
    reApplyEnchantmentEffects();
}

void Card::removeEnchantment(Enchantment* e)
{
    bool found = m_enchantments.removeOne(e);
    Q_ASSERT(found);
    reApplyEnchantmentEffects();
}

void Card::clearEnchantmentEffects()
{
    m_costModifier = 0;

    // The QVectors containers can be deleted, but the inner Trigger* should not:
    // they belong to their relative enchantment that will take care of deleting
    // them!
    qDeleteAll(m_additionalTriggerPowers);
    m_additionalTriggerPowers.clear();
}

void Card::reApplyEnchantmentEffects()
{
    clearEnchantmentEffects();
    foreach (Enchantment* e, m_enchantments) {
        e->apply();
    }

    // Tell the view enchantments might have changed
    IORequest* ir = new IORequest(m_owner, IORequest::ENCHANT_CHARACTER);
    ir->putExtra<Card>("target", this);
    emit m_owner->cardEnchanted(ir);
}

QVector<Enchantment*>* Card::takeEnchantmentsBefore(const Enchantment* s)
{
    Q_ASSERT(m_enchantments.contains(const_cast<Enchantment*>(s)));

    QVector<Enchantment*>* taken = new QVector<Enchantment*>();
    QMutableVectorIterator<Enchantment*> i(m_enchantments);
    while (i.hasNext()) {
        Enchantment* e = i.next();
        if (e == s)
            break;

        i.remove();
        taken->append(e);
    }

    return taken;
}

const QVector<Enchantment*>* Card::enchantments() const
{
    return &m_enchantments;
}
