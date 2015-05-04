#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QtDebug>
#include <QVector>

class Card;
class Character;
class Hero;
class Minion;
class Spell;
class IORequest;
class PlayerInput;
class Action;
class Game;

class Player : public QObject
{
    Q_OBJECT

signals:
    // Asynchronous signals (ir has to be destroyed in connected slot)
    void actionChosen(IORequest* ir);
    void armorAdded(IORequest* ir);
    void attack(IORequest* ir);
    void battlecry(IORequest* ir);
    void characterDamagedOrHealed(IORequest* ir);
    void cardDrawn(IORequest* ir);
    void cardEnchanted(IORequest* ir);
    void cardPutIntoHand(IORequest* ir);
    void cardRemovedFromHand(IORequest* ir);
    void deathRattle(IORequest* ir);
    void drawnCardDestroyed(IORequest* ir);
    void gameEnded(IORequest* ir);
    void manaUpdated(IORequest* ir);
    void minionDestroyed(IORequest* ir);
    void minionSummoned(IORequest* ir);
    void targetChosen(IORequest* ir);

    // Synchronous signals (ir is deleted by emitter)
    void eventTriggered(IORequest* ir);
    void actionSelectionRequired(IORequest* ir);
    void positionSelectionRequired(IORequest* ir);
    void targetSelectionRequired(IORequest* ir);

public:
    Player(Hero* hero, PlayerInput* input, PlayerInput* hisInput,
           const QString& name);
    virtual ~Player();

    void initPlayer();
    void setDeck(const QVector<Card*>& deck, bool shuffleDeck = true);

    void newTurn();
    void drawCard();
    void updateMana(int maxMana, int curMana);
    void modifyCurrentMana(int modification);
    void modifyMaxMana(int modification);
    void putCardInHand(Card* c);
    void removeFromHand(Card* c);
    void win();
    void loose();
    void tie();
    void summonMinion(Minion* m, int position = -1);
    void putCardInGraveyard(Card* c);
    void putKilledMinionToGraveyard(Minion* m);
    void executeAttack(Character* attacker, Character* target);
    void dealDamageHealing(Card* dmgHealingSource, Character* target, int quantity);

    Action* selectAction(const QVector<Action*>* availableActions);
    int selectPosition(const Minion* minionToPosition);
    Character* selectTarget(const QVector<Character*>* availableTargets, Action* a);
    Action* endTurnAction();

    Game* game() const;
    Player* opponent() const;
    const QString& name() const;
    Hero* hero();
    int currentMana() const;
    int maxMana() const;
    int deckSize() const;
    QVector<Card*>* playableCards();
    const QVector<Minion*>* minions() const;
    const QVector<Card*>* hand() const;
    QVector<Character*>* attackableCharacters() const;
    QVector<Character*>* allTargets() const;
    bool hasTauntMinion() const;

protected:
    PlayerInput* m_myInput;
    PlayerInput* m_hisInput;
    Hero* m_hero;
    QVector<Card*> m_deck;
    QVector<Card*> m_graveyard;
    QVector<Card*> m_hand;
    QVector<Minion*> m_minions;
    int m_maxMana;
    int m_curMana;
    Action* m_endTurnAction;
    const QString m_name;
    int m_fatigue;

    void m_shuffleDeck();
};

#endif // PLAYER_H
