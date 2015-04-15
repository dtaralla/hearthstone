#ifndef MINION_H
#define MINION_H

#include <QHash>
#include <QVector>
#include "character.h"
#include "event.h"

class SummonMinionAction;
class AttackAction;

class Minion : public Character
{
public:
    Minion(CharacterIdentity const* baseChar, Player* owner = NULL);
    ~Minion();

    PlayAction* playAction();
    const QVector<Action*>* playActionConsequences() const;
    CardType type() const;

    /**
     * @brief Gets the subtype of this minion.
     *
     * A minion can be many things: an undead creature, a totem, a mech,...
     *
     * It is guaranteed that <code>characterType() &
     * CharacterTypes::MINION</code> returns \c true. It is also guaranteed
     * that this function never returns aggregate subtypes:
     * \li CharacterTypes::MINION
     * \li CharacterTypes::CHARACTER
     *
     * @sa CharacterTypes
     *
     * @return The subtype of this minion.
     */
    CharacterType characterType() const;
    void initCard(Player* cardOwner);
    void setBattlecryResolved(bool resolved);
    bool battlecryResolved() const;

    QVector<Action*>* deathRattles() const;
    const QVector<Action*>* battlecry() const;

private:
    QVector<Action*>* m_playActionConsequences;
    PlayAction* m_playAction;

    QVector<Action*> m_battlecry;
    QVector<Action*> m_deathRattles;
    bool m_battlecryResolved;

};

#endif // MINION_H
