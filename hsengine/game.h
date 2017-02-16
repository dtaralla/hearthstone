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
     * @brief Ends the current player turn and begin the turn of the other
     * player.
     *
     * The sequence executed is the following:
     * -# Unfreeze characters that lost their attack phase because of a
     * FROZEN condition
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

    /**
     * @brief Summons a minion at a given position amongst \a toPlay owner's
     * on-board minions.
     *
     * The minion will not be able to attack this turn (unless it has the
     * CHARGE ability).
     *
     * \a toPlay owner should have less than 7 minions currently on the
     * battlefield.
     *
     * @param toPlay The minion to play (already removed from its owner's hand)
     * @param position The position (between 0 and the number of minions of
     * \a toPlay owner currently on the board included) where to put the
     * summoned minion. Give -1 for the last position.
     */
    void summonMinion(Minion* toPlay, int position = -1);

    /**
     * @brief Triggers an event.
     *
     * When an event fired, each and every card from both players hands and
     * their side of the board will be asked whether or not they listen to the
     * event, heroes included. Then, these cards will execute their event
     * response one by one, in the order in which those cards were put into
     * play. Each action is resolved using updateState(), and thus deaths are
     * resolved between each action (and after all its consequences have been
     * resolved themselves).
     *
     * @param e The event to trigger, with all its parameters set.
     */
    void trigger(const Event& e);

    /**
     * @brief Gets the unique identifier of this game in the process.
     *
     * Can be used for differentiating games in a context where several games
     * execute concurrently (see hsdatabasegenerator module).
     *
     * @return The unique ID of this game
     */
    int id() const;

    /**
     * @brief Gets a list of numbers describing the current state of this game.
     *
     * Let N be the number of possible cards in the game.
     *
     * An environement is composed of:
     * - [0] Integer representing the amount of minions controlled by the
     * current player
     * - [1] Integer representing the amount of minions controlled by the
     * next player
     * - [2] Integer representing the amount of taunts controlled by the
     * current player
     * - [3] Integer representing the amount of taunts controlled by the
     * next player
     * - [4] Integer representing the amount of cards in the current
     * player's hand
     * - [5] Integer representing the amount of cards in the next player's
     * hand
     * - [6] Integer representing the remaining amount of mana the current
     * player has
     * - [7] Integer representing the maximum amount of mana available for
     * the current player
     * - [8] Integer representing the maximum amount of mana available for
     * the next player <em>when it will be his turn</em>
     * - [9] Integer representing the hp of the current player
     * - [10] Integer representing the hp of the next player
     * - [11 to 108] Description of minions on the battlefield: 14 x (*)
     * - [108 to 108+N] Description of the current player's hand; each variable
     * is an integer number representing the amount of cards of ID (i-108) the
     * current player has in his hand.
     *
     * (*): The list of 14 (minion_id, can_attack, current_HP, current_ATK,
     * is_silenced, is_enchanted, is_taunt) tuples might contain missing data
     * in the case where a player does not have 7 minions.
     *
     * @return The list defined hereabove
     */
    QVector<float> environment() const;

    /**
     * @brief Gets the list of attackable targets given an attacking character.
     *
     * According to the game rules, only enemies of \a atker are eligible to be
     * the target of the attack. Moreover, if the enemy owns minions having
     * TAUNT, only these will be attackable. If there are no TAUNT minions, the
     * enemy hero is included in this list.
     *
     * @param atker The attacking character
     *
     * @return The list of attackable characters.
     */
    QVector<Character*>* attackableTargets(const Character* atker) const;

    /**
     * @brief Gets the opponent of a given player.
     *
     * In Hearthstone, there are only 2 players at a given time. This method
     * returns the opponent of \a p.
     *
     * @param p The player whose opponent will be returned
     *
     * @return The opponent of \a p
     */
    Player* opponentOf(const Player* p) const;

    /**
     * @brief Gets the list of cards currently on the board, sorted by play
     * time.
     *
     * Heroes are never present in this list.
     *
     * In Hearthstone, when an event occurs and multiple cards should react to
     * it, the reactions are triggered sequentially, <b>in the order in which
     * the listening cards were put into play</b>. This function can be useful
     * in such cases.
     *
     * Even if for now the only cards possibly on the board are characters, it
     * can also be weapons and secrets, hence the use of this function later
     * once these card types will be implemented.
     *
     * @param p If not \c NULL, the list will only contain cards owned by the
     * \a o of this player. Else, all cards are returned.
     *
     * @param o The owner qualifier on \a p. If equal to ANY_OWNER, all cards
     * are returned.
     *
     * @return A list of cards currently on the board, sorted by play time and
     * belonging to \a o of \a p.
     */
    QVector<Card*>* playedCardsByPlayTime(Player* p = NULL,
                                          Owner o = Owners::ANY_OWNER) const;

    /**
     * @brief charactersByPlayTime Gets the list of characters currently on the
     * board, sorted by play time.
     *
     * Heroes are always the two first characters in this list.
     *
     * In Hearthstone, when an event occurs and multiple cards should react to
     * it, the reactions are triggered sequentially, <b>in the order in which
     * the listening cards were put into play</b>. This function can be useful
     * in such cases.
     *
     * @param p If not \c NULL, the list will only contain characters owned by
     * the \a o of this player. Else, all characters are returned.
     *
     * @param o The owner qualifier on \a p. If equal to ANY_OWNER, all
     * characters are returned.
     *
     * @param ct Only characters of this type will be returned.
     *
     * @return A list of characters of type \a ct currently on the board,
     * sorted by play time, belonging to \a o of \a p.
     */
    QVector<Character*>* charactersByPlayTime(Player* p = NULL,
                                              Owner o = Owners::ANY_OWNER,
                                              CharacterType ct = CharacterTypes::CHARACTER) const;

    /**
     * @brief The BoardControlScore struct represents the detailed score of the
     * board control value function.
     */
    struct BoardControlScore {
        static constexpr const float TOUGH_MINIONS_MULT = 2.5; //!< Multiplier for the "tough minions" factor
        static constexpr const float HAND_MULT = 1;            //!< Multiplier for the "hand" factor
        static constexpr const float MINIONS_MULT = 1.75;      //!< Multiplier for the "minions number" factor
        static constexpr const float SITUATION_MULT = 3.75;    //!< Multiplier for the "situation" factor

        /**
         * @brief The score of the "tough minions" factor, in [-1; 1].
         *
         * Detailed calculation:
         *      = 0    if #AlliedToughMinions == #EnemyToughMinions
         *      = 1/3  if #AlliedToughMinions == #EnemyToughMinions + 1
         *      = 2/3  if #AlliedToughMinions == #EnemyToughMinions + 2
         *      = 3/3  if #AlliedToughMinions >= #EnemyToughMinions + 3
         *      = -1/3 if #AlliedToughMinions == #EnemyToughMinions - 1
         *      = -2/3 if #AlliedToughMinions == #EnemyToughMinions - 2
         *      = -3/3 if #AlliedToughMinions >= #EnemyToughMinions - 3
         */
        float toughMinionsScore;

        /**
         * @brief The score of the "hand" factor, in [-1; 1]
         *
         * Detailed calculation:
         *      = 0    if #allyHand == #enemyHand
         *      = 0.5  if #allyHand == #enemyHand + 1
         *      = 1    if #allyHand >= #enemyHand + 2
         *      = -0.5 if #allyHand == #enemyHand - 1
         *      = -1   if #allyHand <= #enemyHand - 2
         */
        float handScore;

        /**
         * @brief The score of the "minions number" factor, in [-1; 1]
         *
         * Detailed calculation:
         *      = (#allyMinions - #enemyMinions) / (#allyMinions + #enemyMinions)
         */
        float minionsScore;

        /**
         * @brief The score of the "situation" factor, in [-1; 1]
         *
         * Detailed calculation:
         *      - If the best trade the ally can do has a lower value than the
         *        best trade the enemy could do, = -EnemyBestTrade
         *      - Else, = +AllyBestTrade
         */
        float situationScore;

        /**
         * @brief The weighted sum of all factors, in [-1; 1].
         *
         * Detailed calculation:
         *      = SUM of each factor, multiplied by their respective
         *        multiplier, DIVIDED by the sum of all multipliers.
         */
        float score;
    };

    /**
     * @brief Compute the value function of the Board Control meta for the
     * current state of the game.
     *
     * The "ally" player is the current player.
     *
     * @return A BoardControlScore structure detailing the value function
     * value.
     */
    BoardControlScore* meta_BoardControlScore();

    struct AggroScore {
        float enemyHpScore;
        float score;
    };

    AggroScore* meta_AggroScore();


private:
    /**
     * @brief The next game identifier available.
     *
     * Useful in a context where multiple Game objects are instantiated.
     */
    static int mNextGameID;

    /**
     * @brief Whether the game is in database generation mode or not.
     *
     * @sa IsDBGenerationModeOn()
     */
    static bool m_dbGenerationMode;

    /**
     * @brief The unique identifier of this game in the whole program.
     *
     * @sa id()
     */
    int mId;

    /**
     * @brief The player considered as "Player 1".
     *
     * This does not mean "Player 1" is the first player to play.
     */
    Player* m_player1;

    /**
     * @brief The player considered as "Player 2".
     *
     * This does not mean "Player 2" is the second player to play.
     */
    Player* m_player2;

    /**
     * @brief The player who is playing right now.
     */
    Player* m_curPlayer;

    /**
     * @brief The player who will be playing after m_curPlayer ends its turn.
     */
    Player* m_nextPlayer;

    /**
     * @brief A list of all cards placed on the board, ordered by play time.
     */
    QVector<Card*> m_boardCardsByPlayTime;

    /**
     * @brief A queue of minions scheduled to die.
     */
    QVector<Minion*> m_deathsQueue;

    /**
     * @brief Updates the state by resolving an action \a move (maybe in
     * response to an event \a e).
     *
     * Core function of the game. Whenever a minion dies, or when an event is
     * triggered, or when a player chooses to do something, this function is
     * called with the action to execute. This ensures that the stack principle
     * is applied.
     *
     * The stack principle is simple: each action (playing a spell for
     * instance) is considered "resolved" once (and only once) all its
     * consequences are themseves resolved. A very important exception is
     * deaths: they do \b not apply the stack principle. If a minion A dies and
     * has <i>\b Deathrattle: Kill a minion</i>, the death of A is resolved
     * once the killing is done but \b before the killed minion death is
     * resolved. This means that the deathrattles do not chain in a stacked
     * manner: the killed minion deathrattle is \b queued to be executed after
     * all currently registered deathrattles are resolved.
     *
     * Let's now see in details what this method does.
     *
     * First, this method resolves \a move. Once \a move's Action::resolve()
     * method is finished, this checks whether any hero died. This then looks
     * for dying minions in the m_boardCardsByPlayTime list; when it finds one,
     * it appends it to the m_deathsQueue queue and removes it from the
     * m_boardCardsByPlayTime list. The minions removed this way are not able
     * to return to the battlefield anymore (this prevents deathrattles which
     * would heal a minion to heal one of the dying minions). Then,
     * deathrattles are resolved one by one: each action of each deathrattle is
     * resolved using mUpdateState() recursively. As a deathrattle resolution
     * could kill another minion, this function makes sure that only the
     * top-level mUpdateState() call is resolving deathrattles, while
     * lower-levels calls only appends dying minions in the m_deathsQueue.
     * Indeed, the stack principle does \b not apply to death resolutions.
     * Finally, AFTER_XXX-like events are triggered if applicable.
     *
     * The Action::resolve() methods are usually called only from this
     * function. The exception to this rule is for actions based on other
     * actions (like DamageAndFreezeAction). In the course of their resolution,
     * these can call the Action::resolve() method of other actions without
     * going through mUpdateState().
     *
     * @param move The action to resolve.
     *
     * @param e The event triggering action \a move, if any.
     */
    void mUpdateState(Action* move, const Event* e = NULL);

    /**
     * @brief Gets a list of possible attack actions for the current player.
     *
     * In Hearthstone, having a minion on the board does not mean you have
     * always one attack action associated with it. If the minion is exhausted
     * (ie., if it has been put into play this turn), or if the minion is
     * FROZEN, it cannot attack this turn.
     *
     * @return The list of possible attack actions for the current player.
     */
    QVector<Action*>* mAttackActions();

    /**
     * @brief Utility function to add the current player's hand to an
     * environment object as a vector of 10 identifiers.
     *
     * If 2 cards in hand for instance, these are the possible positions:
     * 1 2 / / / / / / / /
     * 1 / 2 / / / / / / /
     * 1 / / 2 / / / / / /
     * 1 / / / 2 / / / / /
     * ...
     * 2 1 / / / / / / / /
     * 2 / 1 / / / / / / /
     * ...
     * / 1 2 / / / / / / /
     * / 1 / 2 / / / / / /
     * ...
     * / 2 1 / / / / / / /
     * / 2 / 1 / / / / / /
     * ...
     *
     * In short: all permutations of 1 2 / / / / / / / /
     * Choose one randomly amongst them -> this way, classificator
     * does not always find the first cards the most important!
     *
     * @param[out] environment The list representing the current environment;
     * this will add 10 identifiers to the list.
     */
    void mAddShuffledHandTo(QVector<float>& environment) const;

    /**
     * @brief Utility function to add the current player's hand to an
     * environment object as a vector of \e N quantities (with \e N the
     * possible cards in the game).
     *
     * Quantity at position \e i the amount of cards of ID \e i the current
     * player has in his hand.
     *
     * @param[out] environment The list representing the current environment;
     * this will add \e N integers to the list.
     */
    void mAddHandAsVectorTo(QVector<float>& environment) const;
};

#endif // GAME_H
