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
