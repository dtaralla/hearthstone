#ifndef CARDINFO_H
#define CARDINFO_H

#include <QString>
#include <QtDebug>
#include <QHash>
#include <QVector>
#include "event.h"
#include "game_globals.h"

class Trigger;

/**
 * @brief The CardInfo class represents a card identity.
 *
 * A card identity is all the base information describing a Card object.
 * Although there can be several Card objects sharing the same identity (for
 * instance, in the case of having 2 Arcane Intellect cards in one's deck),
 * the identity of the card is unique in the program (Arcane Intellect
 * information is common to all Arcane Intellect cards, so there is no need to
 * duplicate this information for each instance of this flyweight).
 *
 * Identities describe what cards \e are but also what cards \e do. However,
 * they usually do so by using \e relative sentences:
 *
 * \li Deal X damage to an <b>enemy</b> target
 * \li Each time <b>you</b> cast a spell, do something
 * \li Each time <b>this</b> minion takes damage, <b>it</b> gains +1 ATK
 * \li ...
 *
 * It means that what a card's identity describes usually refers to things that
 * can be evaluated only by the very instance which is personifiying this
 * identity. It is the reason why all Card objects need to be initialized
 * before being used: they copy the relative information given by their
 * identity and bind it to themselves. Once this is done, they can evaluate the
 * meaning of qualificatives like \e enemy, \e ally or \e this.
 */
class CardInfo
{
public:
    CardInfo(int id, const QString& name, const QString& desc, int manaCost,
             const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
             bool collectible = true);
    virtual ~CardInfo();

    virtual CardType type() const = 0;

    virtual int id() const;
    virtual const QString& name() const;
    virtual const QString& desc() const;
    virtual int manaCost() const;
    virtual bool listensTo(const Event& e) const;
    virtual const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers() const;
    virtual bool collectible() const;

protected:
    int m_id;
    QString m_name;
    QString m_desc;
    int m_manaCost;
    QHash<Event::Type, QVector<Trigger*>* > m_triggerPowers;
    bool m_collectible;
};

QDebug operator<<(QDebug dbg, const CardInfo& c);

#endif // CARDINFO_H
