#ifndef SCRIPTEDPLAYER_H
#define SCRIPTEDPLAYER_H

#include "playerinput.h"
#include <QVector>
class Player;
class Card;
class Character;
class Action;
class Enchantment;

class ScriptedPlayer : public PlayerInput
{
public:
    explicit ScriptedPlayer(QObject* parent = 0);
    virtual ~ScriptedPlayer();
    void attachPlayer(Player* p);

private:
    Player* m_me;
    Player* m_enemy;

    int m_cardScore(Card* c);
    int m_actionScore(Action* a);
    int m_dmgScore(int dmg, Character* onChar);
    int m_healingScore(int healing, Character* onChar);
    int m_silenceScore(Character* onChar);
    int m_enchantScore(const Enchantment* e, Character* onChar);
    int m_freezeScore(Character* onChar);
    int m_attackScore(Character* atker, Character* c);

    Character* m_selectBestAttackTarget(Character* atker, QVector<Character*>* targets);
    Character* m_selectBestDmgSpellTarget(int dmg, QVector<Character*>* targets);
    Character* m_selectBestHealingSpellTarget(int heal, QVector<Character*>* targets);
    Character* m_selectBestSilencingSpellTarget(QVector<Character*>* targets);
    Character* m_selectBestFreezingSpellTarget(QVector<Character*>* targets);
    Character* m_selectBestEnchantmentTarget(const Enchantment* e, QVector<Character*>* targets);

    // PlayerInput interface
public slots:
    void onEventTriggered(IORequest* ir);
    void askForAction(IORequest* ir);
    void askForPosition(IORequest* ir);
    void askForTarget(IORequest* ir);
};

#endif // SCRIPTEDPLAYER_H
