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
    enum TargetType {
        ALLY_HERO,
        BOTH_HEROES,
        ENEMY_HERO,
        EVENT_CHARACTER_PARAMS,
        SELECT_N_IN_GROUP,
        PICK_N_FROM_GROUP,
        TARGET_GROUP,
        TARGET_THIS
    };
    enum EventParamType {
        ATTACKED_CHARACTER,
        ATTACKING_CHARACTER,
        CARD_PLAYED,
        DAMAGED_CHARACTER,
        DYING_MINION,
        HEALED_CHARACTER,
        HEALER,
        PLAYER_ENDING_TURN
    };

    static const int CARD__THE_COIN;
    static const int CARD__EXCESS_MANA;

    static CardDB* Instance();

    ~CardDB();

    CardInfo const* cardInfo(const QString& id) const;
    CardInfo const* cardInfo(int id) const;
    void buildCardDB(const QString& fromFile);
    void buildDeckFromFile(const QString& fromFile, Player* owner,
                           bool shuffleDeck = true);
    void buildRandomDeck(Player* forPlayer);
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
