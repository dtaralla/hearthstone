/**
 * @file game.h
 *
 * @brief This file contains the declaration of the Game class, but also a
 * namespace for predicting/computing possible trades.
 */

#ifndef GAME_H
#define GAME_H

/**
 * @brief Implements the "continue <label>" statement (Java)
 * @param label The loop to continue
 */
#define continue_label(label) goto label

#include <QtDebug>
#include "game_globals.h"

class CardDB;
class Card;
class Character;
class Minion;
class Spell;
class Action;
class Player;
class Event;
class Hero;
class PlayerInput;

/**
 * @brief Aggregates utility functions and types, forming a small framework
 * for computing (an approximation of) the best trade a player could make at a
 * given moment.
 *
 * A @e trade is the result of an attack of an allied minion onto enemy
 * minions. If one can use an ally minion to kill N enemy minions, we say this
 * is a <em>N for 1 trade</em>. One usually wants to maximize N.
 *
 * A trade may also denotes the result of a single attack, and here we mainly
 * use this definition.
 *
 * Trading a 4ATK/1HP ally minion against a 1ATK/1HP enemy minion is not a good
 * trade for us, but it is a good one for the enemy player: trades values are
 * symmetrical and whether it is good or bad depends on which side you are. For
 * this last example, if the 4/1 minion belongs to you, the trade would have a
 * value of -3 for you (you gained 2 points because you killed the enemy 1/1
 * minion and 1 + 1 = 2, but you lost 5 points because this killing was made at
 * the expense of loosing your 4/1 minion while 4 + 1 = 5; therefore the trade
 * value is 2 - 5 = -3).
 */
namespace TradeUtils {
    /**
     * @brief A variable of this type is a simple representation of a minion.
     *
     * This representation only takes into account the remaining HP and ATK of
     * the minion, not information about enchantments nor conditions.
     */
    typedef struct {
        int hp;  //!< The remaining HP of the represented minion
        int atk; //!< The current ATK of the represented minion
    } HP_ATK;

    /**
     * @brief Computes heuristically the value of a very good possible trade.
     *
     * Computes the value of <em>an approximation of<em> the best trade player
     * @a atker can make against player @a target. The @a ignoreHasAttacked
     * parameter specifies whether or not the computation should also take
     * into account allied minions for which the game considers they already
     * attacked (see Character::hasAttacked()).
     *
     * @param atker The player considered as in its attack phase
     * @param target The other player
     * @param ignoreHasAttacked specifies whether or not the computation should
     * also take into account allied minions for which the game considers they
     * already attacked (see Character::hasAttacked())
     *
     * @return The value of the trade approximated and considered as the best
     * one.
     */
    int bestTrade(Player* atker, Player* target, bool ignoreHasAttacked);

    /**
     * @brief Translate a list of minions into a list of minion
     * representations, regardless that they have TAUNT or not.
     *
     * This function is used to get a light list of minions that can be
     * modified without modifying the game data and preventing side effects.
     * The resulting format of such light minions is just their current HP and
     * ATK (see HP_ATK type). This list translates the whole \a minions list,
     * regardless of whether some minions have TAUNT or not.
     *
     * @param[in] minions The original list
     * @param[out] structs A pointer to the list to fill with the translation.
     * The key for a given HP_ATK structure is the sum of its HP_ATK::hp and
     * HP_ATK::atk fields, ensuring \a structs is ordered by minion power.
     */
    void minionsToStructs(const QVector<Minion*>* minions, QMap<int, HP_ATK*>* structs);

    /**
     * @brief Translate a list of minions into lists of minions
     * representations, regarding the absence or presence of some TAUNT
     * minions.
     *
     * This function is used to get light lists of minions that can be
     * modified without modifying the game data and preventing side effects.
     * The resulting format of such light minions is just their current HP and
     * ATK (see HP_ATK type).
     *
     * If one wants to separate attackable minions from others (in the case
     * some have TAUNT), this function can come in handy.
     *
     * @param[in] minions The original list
     * @param[out] atkables \c * \a atkables will be allocated in this function
     * and will contain the list of attackable minions found in \a minions.
     * The key for a given HP_ATK structure is the sum of its HP_ATK::hp and
     * HP_ATK::atk fields, ensuring \a structs is ordered by minion power.
     * @param[out] others If there are TAUNT minions in \a minions,
     * \c * \a others will be allocated in this function and will contain the
     * list of minions from \a minions which do not have TAUNT. If \a minions
     * does not contain any TAUNT, \c * \a others will be set to \c NULL.
     * The key for a given HP_ATK structure is the sum of its HP_ATK::hp and
     * HP_ATK::atk fields, ensuring \a structs is ordered by minion power.
     */
    void tauntablesToStructs(const QVector<Minion*>* minions, QMap<int, HP_ATK*>** atkables, QMap<int, HP_ATK*>** others);

    /**
     * @brief removeItem Remove and destroy (i.e., call delete) the item \a
     * item (which has key \a key) from list \a map.
     *
     * @warning Undefined behavior (likely to crash) if \a item with key \key
     * is not found in \map !
     *
     * @param map The list from which to remove \a item
     * @param key The key with which \a item was put in \a map initially
     * @param item The item to remove (Some items in the list could have the
     * same key, thus the item has to be passed in argument too to be able to
     * remove this very \a item).
     */
    void removeItem(QMap<int, HP_ATK*>* map, int key,  HP_ATK* item);

    /**
     * @brief updateItemPosition Refresh \a item key in \a map so that \a map
     * stays ordered when \a item hp and atk fields have been updated somewhere.
     *
     * @warning Undefined behavior (likely to crash) if \a item with key \key
     * is not found in \map !
     *
     * @param map The list containing the item whose position has to be updated
     * @param key The key \a item was put with initially in \a map
     * @param item The item whose position in \a map should be updated (Some
     * items in the list could have the same key, thus the item has to be
     * passed in argument too to be able to remove this very \a item).
     */
    void updateItemPosition(QMap<int, HP_ATK*>* map, int key, HP_ATK* item);

}

class Game : public QObject
{
    Q_OBJECT
signals:
    void finished();

public slots:
    void enterGameLoop();
    void quitGame();

public:
    static bool IsDBGenerationModeOn();
    static void InitializeGlobals(bool DBGenerationModeOn);

    Game(const QString& p1Name, Hero* p1Hero, const QString& p1Deck, PlayerInput* p1Input,
         const QString& p2Name, Hero* p2Hero, const QString& p2Deck, PlayerInput* p2Input);
    ~Game();

    void initPlayers(QThread* moveTo = NULL);

    void nextTurn();
    void summonMinion(Minion* toPlay, int position = -1);
    void trigger(const Event& e);

    int id() const;
    QVector<float> environment() const;
    QVector<Character*>* attackableTargets(const Character* atker) const;
    Player* opponentOf(const Player* p) const;
    QVector<Card*>* playedCardsByPlayTime(Player* p = NULL,
                                          Owner o = Owners::ANY_OWNER) const;
    QVector<Character*>* charactersByPlayTime(Player* p = NULL,
                                              Owner o = Owners::ANY_OWNER,
                                              CharacterType ct = CharacterTypes::CHARACTER) const;

    struct BoardControlScore {
        static const float TOUGH_MINIONS_MULT = 2;
        static const float HAND_MULT = 1;
        static const float MINIONS_MULT = 0;
        static const float SITUATION_MULT = 3;

        float toughMinionsScore;
        float handScore;
        float minionsScore;
        float situationScore;
        float score;
    };

    BoardControlScore* meta_BoardControlScore();

private:
    static int mNextGameID;
    static bool m_dbGenerationMode;

    int mId;
    Player* m_player1;
    Player* m_player2;
    Player* m_curPlayer;
    Player* m_nextPlayer;
    QVector<Card*> m_boardCardsByPlayTime;
    QVector<Minion*> m_deathsQueue;

    void mUpdateState(Action* move, const Event* e = NULL);
    QVector<Action*>* mAttackActions();
    void mAddShuffledHandTo(QVector<float>& environment) const;
    void mAddHandAsVectorTo(QVector<float>& environment) const;
};

#endif // GAME_H
