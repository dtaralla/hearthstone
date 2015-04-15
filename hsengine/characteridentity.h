#ifndef CHARACTERIDENTITY_H
#define CHARACTERIDENTITY_H

#include "cardidentity.h"

class Action;
class Trigger;

class CharacterIdentity : public CardIdentity
{
public:
    CharacterIdentity(int id,
                  const QString& name, const QString& desc,
                  int manaCost, int hp, int atk,
                  const QVector<Action*>& battlecry,
                  const QVector<Action*>& deathRattles,
                  Ability abilities,
                  const QHash<Event::Type, QVector<Trigger*>*>& triggerPowers,
                  CharacterType characterType = CharacterTypes::GENERAL,
                  bool collectible = true);
    ~CharacterIdentity();

    CharacterType characterType() const;
    CardType type() const;
    int hp() const;
    int atk() const;
    bool hasAbility(Ability a) const;

    const QVector<Action*>* battlecry() const;
    const QVector<Action*>* deathRattles() const;

protected:
    CharacterType m_type;
    int m_hp;
    int m_atk;
    QVector<Action*> m_battlecry;
    QVector<Action*> m_deathRattles;
    Ability m_abilities;
};

#endif // CHARACTERIDENTITY_H