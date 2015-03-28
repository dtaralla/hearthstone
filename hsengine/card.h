#ifndef CARD_H
#define CARD_H

#include <QtDebug>
#include "event.h"
#include "game_globals.h"
#include "stringable.h"

class Player;
class CardInfo;
class Action;
class Trigger;
class PlayAction;
class Enchantment;

class Card : public Stringable
{
public:
    Card(Player* owner = NULL);
    virtual ~Card();

    virtual PlayAction* playAction() = 0;
    virtual const QVector<Action*>* playActionConsequences() const = 0;
    virtual const QString& name() const;
    virtual CardInfo const* base() const = 0;
    virtual CardType type() const = 0;

    virtual void initCard(Player* cardOwner);

    virtual Player* owner() const;
    virtual int effectiveManaCost() const;
    virtual bool listensTo(const Event& e) const;
    virtual QVector<Action*>* actionsOnEvent(const Event& e) const;
    virtual int triggerPowersCount() const;

    virtual void addCostModifier(int mod);
    virtual void addTriggerPowers(const QHash<Event::Type, QVector<Trigger*>*>& tps);

    virtual void addEnchantment(Enchantment* e);
    virtual void removeEnchantment(Enchantment* e);
    virtual void clearEnchantmentEffects();
    virtual void reApplyEnchantmentEffects();
    virtual QVector<Enchantment*>* takeEnchantmentsBefore(const Enchantment* s);
    virtual const QVector<Enchantment*>* enchantments() const;

protected:
    Player* m_owner;
    int m_costModifier;
    QHash<Event::Type, QVector<Trigger*>* > m_triggerPowers;
    QHash<Event::Type, QVector<Trigger*>* > m_additionalTriggerPowers;
    QVector<Enchantment*> m_enchantments;

    // Stringable interface
public:
    virtual QString toString() const;
};

#endif // CARD_H
