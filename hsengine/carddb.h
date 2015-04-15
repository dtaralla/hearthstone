#ifndef CARDDB_H
#define CARDDB_H

#include <QtDebug>
#include <QMap>
#include "game_globals.h"
#include "cardinfo.h"

class SpecialPowerAction;
class SummonMinionAction;
class AttackAction;
class Hero;
class Enchantment;
class TargetExpression;
class GroupExpression;
class Action;

/**
 * @brief The CardDB class is a singleton that manages the database of cards
 * available.
 *
 * Hearthstone is a game mostly defined by the cards it makes possible to play
 * with. In this simulator, cards are defined in an external file as JSON-like
 * objects. This class is able to load and parse these files, along with
 * loading decks. Decks are JSON files too.
 *
 * This class is able to serve cards information when given a card identifier.
 * A card identifier is either a human-readable string or an integer. The
 * information served by this database is read-only and cannot be altered in
 * any way.
 *
 * The database is built by loading a database file whose format is described
 * in the \ref card_database_format appendix.
 *
 * Integer identifiers for cards are related to the order in which the cards
 * are described in the database file. If the database file is modified, the
 * integer identifier of the same card can therefore be altered. The
 * maxCardId() method allows one to get an indication of the number of cards
 * available (attention: this also takes into account heroes' special powers).
 *
 * @ingroup hsengine
 * @sa CardIdentity, Card, card_database_format
 */
class CardDB
{
public:
    /**
     * @brief The TargetType enum contains the possible JARS constants for
     * target expression types that evaluates to characters.
     */
    enum TargetType {
        ALLY_HERO,              //!< The target expression always evaluate to the ally hero.
        BOTH_HEROES,            //!< The target expression always evaluate to both heroes.
        ENEMY_HERO,             //!< The target expression always evaluate to the enemy hero.
        EVENT_CHARACTER_PARAMS, //!< The target expression evaluates to a character-typed event response. @sa EventCharacterParamExpression, Event
        SELECT_N_IN_GROUP,      //!< The target expression evaluates to a selection of character in a character group (the player will be prompted a choice). @sa TargetsFromGroupExpression
        PICK_N_FROM_GROUP,      //!< The target expression evaluates to a random set of characters in a character group. @sa TargetsFromGroupExpression
        TARGET_GROUP,           //!< The target expression evaluates to all characters from a character group. @sa TargetsFromGroupExpression
        TARGET_THIS             //!< The target expression evaluates to the character asking for the target expression to evaluate. @sa TargetThisExpression
    };

    /**
     * @brief The EventParamType enum contains the possible JARS constants for
     * event responses.
     */
    enum EventParamType {
        ATTACKED_CHARACTER,  //!< The attacked character in response to an Event::CHARACTER_ATTACKS.
        ATTACKING_CHARACTER, //!< The attacking character in response to an Event::CHARACTER_ATTACKS.
        CARD_PLAYED,         //!< The card played in response to an Event::BEFORE_CARD_PLAYED or an Event::AFTER_CARD_PLAYED.
        DAMAGED_CHARACTER,   //!< The damaged character in response to an Event::CHARACTER_DAMAGED.
        DYING_MINION,        //!< The dying minion in response to an Event::MINION_DIES.
        HEALED_CHARACTER,    //!< The healed character in response to an Event::CHARACTER_HEALED.
        HEALER,              //!< The healing character in response to an Event::CHARACTER_HEALED.
        PLAYER_ENDING_TURN   //!< The player ending the turn in response to an Event::AT_TURN_END.
    };

    /**
     * @brief The numerical identifier of the "The Coin" card.
     *
     * The Coin is a card unique in a given game. It is given at the beginning
     * of the game, to the player which did not play first.
     */
    static const int CARD__THE_COIN;

    /**
     * @brief The numerical identifier of the "Excess Mana" card.
     *
     * Excess Mana is a special card given in the circumstances where a player
     * should increase its maximum mana reserve while having already reached
     * the 10 mana crystals limit. One Excess Mana card is given per mana
     * that can not be provided.
     */
    static const int CARD__EXCESS_MANA;

    /**
     * @brief Gets the unique instance of the card database.
     *
     * @return The singleton instance
     */
    static CardDB* Instance();

    /**
     * @brief Destructor.
     */
    ~CardDB();

    /**
     * @brief Gets a card identity by its human-readable identifier.
     *
     * The human-readable identifier is the one specified in the loaded JARS
     * database file.
     *
     * A card identity is all the information about a card: its name, text,
     * mana cost, powers,...
     *
     * @param id The card human-readable identifier
     *
     * @return The identity (or flyweight) associated with \a id
     */
    CardIdentity const* cardInfo(const QString& id) const;

    /**
     * @brief Gets a card identity by its numerical identifier.
     *
     * The numerical identifier is defined at the moment the JARS file is
     * loaded and is an internally-managed value.
     *
     * A card identity is all the information about a card: its name, text,
     * mana cost, powers,...
     *
     * @param id The card numerical identifier
     *
     * @return The identity (or flyweight) associated with \a id
     */
    CardIdentity const* cardInfo(int id) const;

    /**
     * @brief Loads a JARS file containing the cards you want to make available
     * in the games spawned by this process.
     *
     * @param fromFile The path to the JARS file to load.
     */
    void buildCardDB(const QString& fromFile);

    /**
     * @brief Loads the deck described in \a fromFile, and attaches it to
     * \a owner.
     *
     * Deck files are JSON files with the following syntax: the root object is
     * an array of objects, and each object is of the form
     * \code{.json}
     * {
     *     "id": String,
     *     "quantity": Integer
     * }
     * \endcode
     * where
     * \li the value of the \c id field is the human-readable identifier of
     * a card which was part of the JARS file previously loaded with
     * buildCardDB().
     * \li the value of the \c quantity field is the quantity of these cards
     * that will be put in the deck.
     *
     * For a deck to be valid, all its cards should be \c collectible.
     * Moreover, the deck should contain exactly 30 cards. Even though in the
     * real Hearthstone a deck cannot feature more than 2 cards sharing the
     * same identity, this restriction does not apply in this library.
     *
     * @param fromFile The file containing the deck description.
     *
     * @param owner The player the deck will be attached to.
     *
     * @param shuffleDeck Whether the deck loaded from the file should be
     * suffled or not. If \c false, the cards will be put in the reverse order
     * in which they are found in \a fromFile, the first one being the last
     * card that could be drawn.
     */
    void buildDeckFromFile(const QString& fromFile, Player* owner,
                           bool shuffleDeck = true);

    /**
     * @brief Builds a totally random deck based on the collectible cards
     * found in the loaded database and attaches it to \a forPlayer.
     *
     * The deck is guaranteed to only contain collectible cards, and never more
     * than 2 cards sharing the same identity.
     *
     * @param forPlayer The player the deck will be attached to.
     */
    void buildRandomDeck(Player* forPlayer);

    /**
     * @brief Builds a Hero object based on a human-readable character
     * identifier.
     *
     * The identifier provided should identifiy a HeroInfo object from the
     * database. Practically, this identifier should be present in the JARS
     * file loaded, and the card described with this id should have its
     * \c type field set to \c CARD_HERO.
     *
     * @param id The identifier of the hero to build.
     *
     * @return The built hero.
     */
    Hero* buildHero(const QString& id);

    /**
     * @brief Gets the maximum value amongst all card IDs available.
     *
     * @return The maximum value amongst all card IDs available.
     */
    int maxCardId() const;

private:
    /**
     * @brief The unique database object.
     */
    static CardDB* mInstance;

    /**
     * @brief An associative list mapping numerical identifiers to card
     * identities.
     */
    QMap<int, CardIdentity const*> mCardFlyweights;

    /**
     * @brief An associative list mapping human-readable card identifiers to
     * the corresponding numerical identifier.
     */
    QHash<QString, int> mCardIDsTranslationTable;

    /**
     * @brief An associative list mapping JARS textual constants to
     * recognized corresponding integers.
     */
    QHash<QString, int> mTextIDsTranslationTable;

    /**
     * @brief The next available action ID.
     */
    int mNextActionID;

    /**
     * @brief The next available numerical card ID.
     */
    int mNextCardID;


    /**
     * @brief Constructor.
     */
    CardDB();

    /**
     * @brief Initializes the mTextIDsTranslationTable.
     */
    void mInitTextIDsTranslationTable();

    /**
     * @brief Translates an event parameter JARS constant into the regular
     * event param type.
     *
     * @param ep The JARS constant to translate.
     *
     * @return The corresponding event param type.
     */
    EventParam mIdToEventParam(EventParamType ep);

    /**
     * @brief Parses a JARS \c action object.
     *
     * The \a eType allows semantic checks to be performed while parsing.
     *
     * @param action The JARS \c action object to parse.
     *
     * @param eType In the case where the \c action object is enclosed (at some
     * JARS level) into a \c trigger object, this argument should have the same
     * value than the \c event field of the closest enclosing \c trigger. If
     * this \c action object is not enclosed in any \c trigger, this field is
     * ignored.
     *
     * @return The Action object corresponding to \a action.
     */
    Action* mParseAction(const QJsonObject& action, Event::Type eType = Event::NO_EVENT);

    /**
     * @brief Parses an array of JARS \c action objects.
     *
     * The \a eType allows semantic checks to be performed while parsing.
     *
     * @param actions An array of JARS \c action objects to parse.
     *
     * @param eType In the case where this \c action object list is enclosed
     * (at some JARS level) into a \c trigger object, this argument should have
     * the same value than the \c event field of the closest enclosing \c
     * trigger. If this \c action object is not enclosed in any \c trigger,
     * this field is ignored.
     *
     * @return A list of Action objects corresponding to the ones contained in
     * \a actions, in the same order.
     */
    QVector<Action*>* mParseActionList(const QJsonValue& actions, Event::Type eType = Event::NO_EVENT);

    /**
     * @brief Parses JARS \c ability constants.
     *
     * @param abs The \c ability constant to parse; may be an array of \c
     * ability constants.
     *
     * @return An ORed Ability value, which is the combination of all abilities
     * given in \a abs.
     */
    Ability mParseAbilities(const QJsonValue& abs);

    /**
     * @brief Parses a JARS \c trigger object.
     *
     * @param trigger The JARS \c trigger to parse.
     *
     * @return The Trigger object corresponding to \a trigger.
     */
    Trigger* mParseTrigger(const QJsonObject& trigger);

    /**
     * @brief Parses JARS \c characterType constants.
     *
     * @param ct The \c characterType to parse; may be an array of \c
     * characterType constants.
     *
     * @param defaultType The type to return if \a ct is not parsable as a JARS
     * \c characterType constant.
     *
     * @return An ORed CharacterType value, which is the combination of all
     * character types given in \a ct. If \c ct cannot be parsed as a JARS
     * \c characterType constant, it returns \c defaultType.
     */
    CharacterType mParseCharacterType(const QJsonValue& ct, CharacterType defaultType);

    /**
     * @brief Parses a JARS \c enchantment object.
     *
     * @param e The JARS \c enchantment to parse.
     *
     * @return The Enchantment object corresponding to \a e.
     */
    Enchantment* mParseEnchantment(const QJsonObject& e);

    /**
     * @brief Parses a JARS \c triggerPower object and put it in a trigger
     * power table.
     *
     * If the Event::Type corresponding to the value of the \c event field of
     * \a trgPower is already a key of \a powerTable, the trigger power will be
     * added to the list which is mapped by this key. In the other case, the
     * pair <\c event field value, \a trgPower> is inserted in \a powerTable.
     *
     * @param trgPower The JARS \c triggerPower object to parse.
     *
     * @param powerTable The trigger power table in which to insert the parsed
     * object.
     */
    void mParseTriggerPower(const QJsonObject& trgPower, QHash<Event::Type, QVector<Trigger*>*>* powerTable);

    /**
     * @brief Parses a JARS \c specialPower object.
     *
     * @param sp The JARS \c specialPower to parse.
     *
     * @return The SpecialPower object corresponding to \a sp.
     */
    SpecialPowerAction* mParseSpecialPower(const QJsonObject& sp);

    /**
     * @brief Parses a JARS \c targetExpression object.
     *
     * @param target The JARS \c targetExpression object to parse.
     *
     * @param eType In the case where this \c targetExpression object is
     * enclosed (at some JARS level) into a \c trigger object, this argument
     * should have the same value than the \c event field of the closest
     * enclosing \c trigger. If this \c targetExpression object is not enclosed
     * in any \c trigger, this field is ignored.
     *
     * @return The TargetExpression object corresponding to \a target.
     */
    TargetExpression* mParseTarget(const QJsonObject& target, Event::Type eType = Event::NO_EVENT);

    /**
     * @brief Parses a JARS \c groupExpression object.
     *
     * @param group The JARS \c groupExpression object to parse.
     *
     * @param eType In the case where this \c groupExpression object is
     * enclosed (at some JARS level) into a \c trigger object, this argument
     * should have the same value than the \c event field of the closest
     * enclosing \c trigger. If this \c groupExpression object is not enclosed
     * in any \c trigger, this field is ignored.
     *
     * @return The GroupExpression object corresponding to \a group.
     */
    GroupExpression mParseGroup(const QJsonObject& group, Event::Type eType = Event::NO_EVENT);
};

#endif // CARDDB_H
