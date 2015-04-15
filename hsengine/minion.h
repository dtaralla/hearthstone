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
