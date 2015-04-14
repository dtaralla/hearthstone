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
    CardInfo const* cardInfo(const QString& id) const;

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
    CardInfo const* cardInfo(int id) const;

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
     * @brief Builds a
     * @param id
     * @return
     */
    Hero* buildHero(const QString& id);
    int maxCardId() const;

private:
    static CardDB* mInstance;
    QMap<int, CardInfo const*> mCardFlyweights;
    QHash<QString, int> mCardIDsTranslationTable;
    QHash<QString, int> mTextIDsTranslationTable;
    int mNextActionID;
    int mNextCardID;


    CardDB();
    void mInitTextIDsTranslationTable();
    EventParam mIdToEventParam(EventParamType ep);

    Action* mParseAction(const QJsonObject& action, Event::Type eType = Event::NO_EVENT);
    QVector<Action*>* mParseActionList(const QJsonValue& actions, Event::Type eType = Event::NO_EVENT);
    Ability mParseAbilities(const QJsonValue& abs);
    Trigger* mParseTrigger(const QJsonObject& trigger);
    CharacterType mParseMinionType(const QJsonValue& ct);
    CharacterType mParseTargetType(const QJsonValue& ct);
    Enchantment* mParseEnchantment(const QJsonObject& e);
    void mParseTriggerPower(const QJsonObject& trgPower, QHash<Event::Type, QVector<Trigger*>*>* powerTable);
    SpecialPowerAction* mParseSpecialPower(const QJsonObject& sp);
    TargetExpression* mParseTarget(const QJsonObject& target, Event::Type eType = Event::NO_EVENT);
    GroupExpression mParseGroup(const QJsonObject& group, Event::Type eType = Event::NO_EVENT);
};

#endif // CARDDB_H
