/**
 * @file game.h
 *
 * @brief This file contains the declaration of the Game class, but also a
 * namespace for predicting/computing possible trades.
 */

/**
 * @defgroup hsengine Hearthstone Simulator Engine
 *
 * @brief This module regroups all classes and files relative to the game
 * engine.
 *
 * This module contains all files needed to build a standalone library of the
 * game engine.
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

/**
 * @brief The Game class represents the whole game context and presents an API
 * to others classes to interact with it.
 *
 * The game can be in one of two modes: Database Generation mode and Play mode.
 * \li In Database Generation mode, the game will use a DBOutput instance to
 * output a database of targeted actions taken. Play actions won't be recorded;
 * for that you should use the \ref hsdatabasegenerator shipped with this
 * engine.
 * \li In Play mode, the game is used to simulate Hearthstone, without any
 * output.
 *
 * The Game class should always be initialized using InitializeGlobals() at the
 * beginning of any program.
 *
 * After creating a Game object \c g, one should create a GameController object
 * with \c g as constructor argument. One can also create a GameController
 * with no arguments and use GameController::setGame() later with \c g as
 * argument. See the sources of the GameThreadPool for an example of this last
 * case.
 *
 * Before entering the game loop, initPlayers() should be called with the
 * thread in which the object will live once started. This way, players are
 * living in the same thread than the game object.
 *
 * Once everything is in place, emit a signal linked to the enterGameLoop()
 * slot. This will start the game loop, i.e. the game will begin.
 *
 * @warning A Game object should \e always be encapsulated into a
 * GameController that will take care of managing the thread in which the Game
 * object and the players will live.
 *
 * @warning A Game object should \e always live in another thread than the main
 * Qt event loop. This is managed automatically by the GameController it is
 * linked to. The thread in which the Game object lives <em>has to be</em> a
 * GameThread.
 *
 * @sa GameController, GameThread, hsdatabasegenerator
 *
 * @ingroup hsengine
 */
class Game : public QObject
{
    Q_OBJECT
signals:
    /**
     * @brief This signal is emitted by the game after the result of the game
     * are sent to the players' PlayerInput's.
     *
     * The game is considered as finished at the time this signal is emitted.
     */
    void finished();

public slots:
    /**
     * @brief Start the execution of the game.
     *
     * @warning Make sure that:
     * \li this object is living in another thread than the Qt event loop.
     * \li the thread in which this object lives as an event loop running.
     * \li initPlayers() has been called before this object was moved to the
     * thread where it is going to execute.
     *
     * See the documentation of the Game class for more details.
     */
    void enterGameLoop();

public:
    /**
     * @brief Tells whether the current mode is the Database Generation mode.
     *
     * See the documentation of the Game class for more details about game
     * modes.
     *
     * @return \c true if in Database Generation mode, \c false otherwise.
     */
    static bool IsDBGenerationModeOn();

    /**
     * @brief Initialize global variables used in the \ref hsengine.
     *
     * @warning Should always be called at the very beginning of any program
     * linked to this library.
     *
     * @param DBGenerationModeOn Whether or not the program is going to be in
     * Database Generation mode.
     */
    static void InitializeGlobals(bool DBGenerationModeOn);

    /**
     * @brief Constructor.
     *
     * Creates a game, initializing its players and deciding which one of them
     * will go first.
     *
     * @param p1Name Player 1's name.
     * @param p1Hero Player 1's Hero.
     * @param p1Deck Player 1's deck file. This should be the complete
     * filename of the JSON file containing the deck you want to attach to
     * Player 1. If an empty string is given, a random deck will be built and
     * attached (see CardDB::buildRandomDeck()).
     * @param p1Input Player 1's PlayerInput.
     * @param p2Name Player 2's name.
     * @param p2Hero Player 2's Hero.
     * @param p2Deck Player 2's deck file. This should be the complete
     * filename of the JSON file containing the deck you want to attach to
     * Player 2. If an empty string is given, a random deck will be built and
     * attached (see CardDB::buildRandomDeck()).
     * @param p2Input Player 2's PlayerInput.
     */
    Game(const QString& p1Name, Hero* p1Hero, const QString& p1Deck, PlayerInput* p1Input,
         const QString& p2Name, Hero* p2Hero, const QString& p2Deck, PlayerInput* p2Input);

    /**
     * @brief Destructor.
     *
     * Deletes Player 1, then Player 2. If any DBOutput instance was linked to
     * this game (if in Database Generation mode), it will be deleted too.
     */
    ~Game();

    /**
     * @brief Initialize the players of the game.
     *
     * Move the players into the same thread than this Game object, then
     * call their Player::initPlayer() methods.
     *
     * @warning This should always be called \e after the game has been moved
     * to the thread in which it is going to be executed.
     * @warning This should always be called \e before starting the game.
     * @warning If this method is called from the \a moveTo thread, it has an
     * undefined behavior.
     */
    void initPlayers();

    /**
     * @brief End the current player turn and begin the turn of the other
     * player.
     *
     * The sequence executed is the following:
     * -# Unfreeze characters that lost their attack phase because of a
     * Abilities::FROZEN condition
     * -# Trigger a Event::AT_TURN_END with \c player set to the player whose
     * turn is ended
     * -# Switch <em>current player</em> and <em>next player</em>
     * -# Call the Player::nextTurn() method of the new current player
     *
     * @triggers
     * Event::AT_TURN_END with
     *      - \c player : the player whose turn is ended
     */
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
