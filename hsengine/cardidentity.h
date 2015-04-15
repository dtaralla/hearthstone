#ifndef CARDIDENTITY_H
#define CARDIDENTITY_H

#include <QString>
#include <QtDebug>
#include <QHash>
#include <QVector>
#include "event.h"
#include "game_globals.h"

class Trigger;

/**
 * @brief The CardIdentity class represents a card identity.
 *
 * A card identity is all the base information describing a Card object.
 * Although there can be several Card objects sharing the same identity (for
 * instance, in the case of having 2 Arcane Intellect cards in one's deck),
 * the identity of the card is unique in the program (Arcane Intellect
 * information is common to all Arcane Intellect cards, so there is no need to
 * duplicate this information for each instance of this identity).
 *
 * Identities describe what cards \e are but also what cards \e do. However,
 * they usually do so by using \e relative qualifiers:
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
 * meaning of qualifiers like \e enemy, \e ally or \e this.
 *
 * Usually, CardIdentity objects are created based on information organized as a
 * JARS file imported by the CardDB singleton.
 *
 * To allow one to know which kind of identity it is actually dealing with, the
 * type() factory method is provided. This method is implemented in child
 * classes of CardIdentity and returns a CardType. You might wander whether a
 * CardIdentity is a CharacterInfo, regardless of it being a HeroInfo or a minion's
 * identity. As the CardType is a bitstring, you can check this by using
 * <code>info->type() & CardTypes::CARD_CHARACTER</code>.
 *
 * As we said, there are multiple card identity types and each has its
 * dedicated characteristics. However, all card identity types share a set of
 * characteristics: they present a type(), can provide their id(), their
 * manaCost(), which kind of events they listen to,... All card identities can
 * also be collectible or not. Because of all those shared characteristics
 * amongst card identities, this superclass is the parent of all other card
 * identity classes and provides a base implementation for all these common
 * things.
 *
 * @ingroup hsengine
 * @sa Card, CardDB
 */
class CardIdentity
{
public:
    /**
     * @brief Constructor.
     *
     * @param id The unique numerical identifier of this identity.
     * @param name The card name that could possibly be displayed in a UI
     * @param desc The card description that could possibly be displayed in a
     * UI
     * @param manaCost The base mana cost of all cards having this identity
     * @param triggerPowers An associative list containing the base trigger
     * powers of all cards having this identity
     * @param collectible Whether cards with this identity can be put in a deck
     */
    CardIdentity(int id, const QString& name, const QString& desc, int manaCost,
             const QHash<Event::Type, QVector<Trigger*>* >& triggerPowers,
             bool collectible = true);

    /**
     * @brief Destructor.
     */
    virtual ~CardIdentity();

    /**
     * @brief Gets this card identity's type.
     *
     * When you only have a <code>CardIdentity*</code> pointer, this factory method
     * can come in handy to determine the effective type of the pointed object.
     *
     * If your are not interested in the precise type of this card, you might
     * still be wandering whether a CardIdentity is a CharacterInfo, regardless of
     * it being information about a Hero or a Minion. As the CardType is a
     * bitstring, you can check this by using a test like <code>info->type() &
     * CardTypes::CARD_CHARACTER</code>.
     *
     * @sa CardType, CardTypes
     *
     * @return The type of card this identity represents.
     */
    virtual CardType type() const = 0;

    /**
     * @brief Gets the unique numerical identifier of this identity.
     *
     * @return The unique numerical identifier of this identity.
     */
    virtual int id() const;

    /**
     * @brief Gets the card name.
     *
     * This function can be used if one wants to display the card in a UI.
     *
     * @return The card name.
     */
    virtual const QString& name() const;

    /**
     * @brief Gets the card description.
     *
     * This function can be used if one wants to display the card in a UI.
     *
     * The card description usually is a rich text string containing in
     * <b>bold</b> the abilities of the card along with explanatory text about
     * its trigger powers.
     *
     * Example: "<b>Taunt</b>. <b>Divine Shield</b>. Each time this minion
     * deals damage, <b>freeze</b> the target."
     *
     * @return The card description.
     */
    virtual const QString& desc() const;

    /**
     * @brief Gets the base mana cost of all cards having this identity
     * @return The base mana cost of all cards having this identity. This can
     * never be negative.
     */
    virtual int manaCost() const;

    /**
     * @brief Gets the associative list containing the base triggers powers of
     * all cards having this identity.
     *
     * @return The associative list containing the base triggers powers of all
     * cards having this identity.
     */
    virtual const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers() const;

    /**
     * @brief Tells whether cards having this identity are collectible or not.
     *
     * A collectible card is a card which can appear in a deck description.
     * Non-collectible cards cannot appear in deck description.
     *
     * Non-collectible cards are for instance minions created by spells, or
     * even heroes.
     *
     * @return Whether cards having this identity are collectible or not.
     */
    virtual bool collectible() const;

protected:
    /**
     * @brief The unique numerical identifier of this card identity.
     * @sa id()
     */
    int m_id;

    /**
     * @brief The card name.
     * @sa name()
     */
    QString m_name;

    /**
     * @brief The card description.
     * @sa desc()
     */
    QString m_desc;

    /**
     * @brief The base mana cost of all cards sharing this identity.
     * @sa manaCost()
     */
    int m_manaCost;

    /**
     * @brief An associative list mapping event types to a list of triggers.
     * @sa triggerPowers()
     */
    QHash<Event::Type, QVector<Trigger*>* > m_triggerPowers;

    /**
     * @brief Whether the card is collectible or not.
     * @sa collectible()
     */
    bool m_collectible;
};

QDebug operator<<(QDebug dbg, const CardIdentity& c);

#endif // CARDIDENTITY_H
