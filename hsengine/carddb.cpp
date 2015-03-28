#define FORCE_CARDLIST_REFRESH 0

#include "carddb.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtDebug>
#include <QRegularExpression>
#include "game.h"
#include "characterinfo.h"
#include "spellinfo.h"
#include "heroinfo.h"
#include "card.h"
#include "minion.h"
#include "spell.h"
#include "player.h"
#include "hero.h"
#include "enchantment.h"
#include "actions/cardactions.h"
#include "triggers/minionkilledtrigger.h"
#include "triggers/turnendtrigger.h"
#include "triggers/characterdamagedtrigger.h"
#include "triggers/cardplayedtrigger.h"
#include "expressions/target/targetsfromgroupexpression.h"
#include "expressions/target/targetthisexpression.h"
#include "expressions/target/eventcharacterparamexpression.h"
#include "expressions/group/groupexpression.h"

CardDB* CardDB::mInstance = NULL;
const int CardDB::CARD__THE_COIN = 0;
const int CardDB::CARD__EXCESS_MANA = 1;

CardDB* CardDB::Instance()
{
    if (mInstance == NULL)
        mInstance = new CardDB();
    return mInstance;
}

CardDB::~CardDB()
{
    qDeleteAll(mCardFlyweights);
}

CardInfo const* CardDB::cardInfo(const QString& id) const
{
    CardInfo const* f = mCardFlyweights.value(mCardIDsTranslationTable.value(id), NULL);
    if (f == NULL)
        qWarning() << "There were no cards with id '" << id << "' in current DB";

    return f;
}

CardInfo const* CardDB::cardInfo(int id) const
{
    CardInfo const* f = mCardFlyweights.value(id, NULL);
    if (f == NULL)
        qWarning() << "There were no cards with id '" << id << "' in current DB";

    return f;
}

void CardDB::buildCardDB(const QString& fromFile)
{
    if (!mCardFlyweights.empty()) {
        qWarning() << "Cannot import a card database if one is already present";
        return;
    }


    // Read file
    QFile db(fromFile % ".json");
    if (!db.exists() || FORCE_CARDLIST_REFRESH) {
        qWarning() << fromFile << "cache has to be created...";

        QFile oriDb(fromFile % ".card");
        bool opened = oriDb.open(QFile::ReadOnly);
        Q_ASSERT(opened);

        QTextStream in(&oriDb);
        QString content = in.readAll();
        oriDb.close();

        mInitTextIDsTranslationTable();
        foreach (QString key, mTextIDsTranslationTable.keys())
            content = content.replace(key.prepend('$'), QString("%1").arg(mTextIDsTranslationTable.value(key)));
        mTextIDsTranslationTable.clear();
        QRegularExpression reg("\\$[a-zA-Z0-9_]*");
        QRegularExpressionMatchIterator i = reg.globalMatch(content);
        while (i.hasNext())
            qWarning() << "Unresolved token:" << i.next().captured();

        reg = QRegularExpression("[0-9]+_[a-zA-Z0-9_]*");
        i = reg.globalMatch(content);
        while (i.hasNext())
            qWarning() << "Misreplaced token:" << i.next().captured();

        opened = db.open(QFile::WriteOnly);
        Q_ASSERT(opened);
        QTextStream out(&db);
        out << content;
        db.close();

        qWarning() << "Cache written!";
    }
    bool opened = db.open(QFile::ReadOnly);
    Q_ASSERT(opened);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(db.readAll(), &error);
    db.close();
    if (error.error != QJsonParseError::NoError)
        qCritical() << error.errorString();

    // Create db...
    // Add "the coin"
    mCardFlyweights.insert(CARD__THE_COIN,
                        new SpellInfo(CARD__THE_COIN, "The Coin",
                                      "Gives you +1 Mana Crystal until the end of this turn.",
                                      0, new QVector<Action*>(1, new AddManaAction(Owners::ALLY, 1)),
                                      QHash<Event::Type, QVector<Trigger*>*>(), false));
    mCardIDsTranslationTable.insert("THE_COIN", CARD__THE_COIN);

    // Add "excess mana"
    mCardFlyweights.insert(CARD__EXCESS_MANA,
                        new SpellInfo(CARD__EXCESS_MANA, "Excess Mana",
                                      "Draw a card. <i>(You can only have 10 mana in your tray.)</i>",
                                      0, new QVector<Action*>(1, new DrawCardAction()),
                                      QHash<Event::Type, QVector<Trigger*>*>(), false));
    mCardIDsTranslationTable.insert("EXCESS_MANA", CARD__EXCESS_MANA);

    Q_ASSERT(doc.isArray());
    foreach (QJsonValue v, doc.array()) {
        QJsonObject o = v.toObject();
        if (!o.contains("type"))
            qCritical() << "Object" << o << "has no 'type' attribute.";

        int type = o.value("type").toInt(-1);
        if (type == -1)
            qCritical() << "Object" << o << "type is not an integer";

        // Parse key
        if (!o.value("id").isString())
            qCritical() << "Value" << o.value("id") << "is no string";
        QString key = o.value("id").toString("invalid_string");
        if (mCardIDsTranslationTable.contains(key))
            qCritical() << "DB already contains id" << key << "; obj was" << o;

        // Parse trigger powers
        QHash<Event::Type, QVector<Trigger*>* > triggerPowers;
        if (type == CardTypes::CARD_MINION || type == CardTypes::CARD_SPELL) {
            QJsonValue tps = o.value("triggerPowers");
            if (!tps.isUndefined()) {
                if (tps.isArray()) {
                    foreach (QJsonValue v2, tps.toArray())
                        mParseTriggerPower(v2.toObject(), &triggerPowers);
                }
                else
                    mParseTriggerPower(tps.toObject(), &triggerPowers);
            }
        }

        CardInfo* toInsert;
        if (type == CardTypes::CARD_MINION) {
            // Parse death rattles
            QVector<Action*> deathRattles;
            QJsonValue drs = o.value("deathrattles");
            if (!drs.isUndefined()) {
                if (drs.isArray()) {
                    foreach (QJsonValue v2, drs.toArray()) {
                        if (!v2.isObject())
                            qCritical() << "Value" << v2 << "is not a JSON object.";
                        deathRattles.append(mParseAction(v2.toObject()));
                    }
                }
                else
                    deathRattles.append(mParseAction(drs.toObject()));
            }


            // Parse abilities
            Ability abilities = Abilities::NO_ABILITY;
            QJsonValue abs = o.value("abilities");
            if (abs.isArray()) {
                foreach (QJsonValue v2, abs.toArray()) {
                    if (!v2.isDouble())
                        qCritical() << "Value" << v2 << "is not an identifier (int).";
                    abilities |= v2.toInt(Abilities::NO_ABILITY);
                }
            }
            else
                abilities = o.value("abilities").toInt(Abilities::NO_ABILITY);

            // Parse battlecry
            QVector<Action*> battlecry;
            QJsonValue bcs = o.value("battlecry");
            if (!bcs.isUndefined()) {
                if (bcs.isArray()) {
                    foreach (QJsonValue v2, bcs.toArray()) {
                        if (!v2.isObject())
                            qCritical() << "Value" << v2 << "is not a JSON object.";
                        battlecry.append(mParseAction(v2.toObject()));
                    }
                }
                else
                    battlecry.append(mParseAction(bcs.toObject()));
            }


            // Parse subtype
            CharacterType subtype = mParseMinionType(o.value("subtype"));


            // Create DB entry
            toInsert = new CharacterInfo(
                mNextCardID++,
                o.value("name").toString("invalid_string"),
                o.value("text").toString("invalid_string"),
                o.value("cost").toInt(-1),
                o.value("health").toInt(-1),
                o.value("attack").toInt(-1),
                battlecry,
                deathRattles,
                abilities,
                triggerPowers,
                subtype,
                o.value("collectible").toBool(true));
        }
        else if (type == CardTypes::CARD_SPELL) {
            toInsert = new SpellInfo(
                mNextCardID++,
                o.value("name").toString("invalid_string"),
                o.value("text").toString("invalid_string"),
                o.value("cost").toInt(-1),
                mParseActionList(o.value("actions")),
                triggerPowers,
                o.value("collectible").toBool(true));
        }
        else if (type == CardTypes::CARD_HERO) {
            toInsert = new HeroInfo(
                mNextCardID++,
                o.value("name").toString("invalid_string"),
                mParseSpecialPower(o.value("power").toObject()));
        }
        else
            qCritical() << "Unrecognized type for object" << o;

        mCardFlyweights.insert(toInsert->id(), toInsert);
        mCardIDsTranslationTable.insert(key, toInsert->id());
    }

    // Write empty deck
    QFile emptyDeck(fromFile + ".emptyDeck");
    if (FORCE_CARDLIST_REFRESH || !emptyDeck.exists()) {
        opened = emptyDeck.open(QFile::WriteOnly);
        Q_ASSERT(opened);
        QTextStream out(&emptyDeck);
        out << "[";
        foreach (const CardInfo* ci, mCardFlyweights.values()) {
            if (!ci->collectible())
                continue;
            out << "{\n  \"id\": \"" << ci->id() << ",\n  \"quantity\": 0\n},";
        }
        emptyDeck.close();
    }

    qDebug() << mCardFlyweights.size() << "cards available";
}

void CardDB::buildDeckFromFile(const QString& fromFile, Player* owner,
                               bool shuffleDeck)
{
    Q_CHECK_PTR(owner);

    // Read file
    QFile db(fromFile);
    bool opened = db.open(QFile::ReadOnly);
    Q_ASSERT(opened);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(db.readAll(), &error);
    if (error.error != QJsonParseError::NoError)
        qCritical() << error.errorString();

    // Create deck...
    Q_ASSERT(doc.isArray());
    QVector<Card*> deck;
    deck.reserve(30);
    foreach (QJsonValue v, doc.array()) {
        if (!v.isObject())
            qCritical() << "Value" << v << "is not a JSON object.";

        QJsonObject o = v.toObject();
        int q = o.value("quantity").toInt(1);
        if (q <= 0)
            continue;

        if (!o.contains("id") || !o.value("id").isString())
            qCritical() << "Object" << o << "has no valid 'id' attribute.";

        if (o.contains("quantity") && !o.value("quantity").isDouble())
            qCritical() << "Object" << o << "has no valid 'quantity' attribute.";

        const CardInfo* info = mCardFlyweights.value(mCardIDsTranslationTable.value(o.value("id").toString()), NULL);
        if (info == NULL)
            qCritical() << "ID" << o.value("id").toString() << "was not found in database.";

        if (!info->collectible())
            qCritical() << "Card" << *info << "is NOT collectible!";

        switch (info->type()) {
            case CardTypes::CARD_MINION:
                for (int i = 0; i < q; i += 1) {
                    Card* c = new Minion((const CharacterInfo*) info);
                    c->initCard(owner);
                    deck << c;
                }
                break;

            case CardTypes::CARD_SPELL:
                for (int i = 0; i < q; i += 1) {
                    Card* c = new Spell((const SpellInfo*) info);
                    c->initCard(owner);
                    deck << c;
                }
                break;

            case CardTypes::CARD_HERO:
                qCritical() << "You cannot have a hero card in your deck!";
                break;

            default:
                qCritical() << "Only minions and spells are supported for now.";
        }
    }

    if (deck.size() != 30)
        qCritical() << "A deck is composed of 30 cards, not" << deck.size();

    owner->setDeck(deck, shuffleDeck);
}

void CardDB::buildRandomDeck(Player* forPlayer)
{
    QVector<Card*> deck;
    QList<const CardInfo*> db = mCardFlyweights.values();
    int amounts[db.size()] = { 0 };
    const int size = db.size() - 2; // exclude coin, excess mana
    deck.reserve(30);
    while (deck.size() < 30) {
        // Pick a card
        int pickAt = 2 + qrand() % size;
        const CardInfo* pick = db.at(pickAt);
        if (!pick->collectible() || amounts[pickAt] >= 2)
            continue;

        amounts[pickAt] += 1;
        Card* c;
        switch (pick->type()) {
            case CardTypes::CARD_MINION:
                c = new Minion((const CharacterInfo*) pick);
                break;

            case CardTypes::CARD_SPELL:
                c = new Spell((const SpellInfo*) pick);
                break;

            default:
                qCritical() << "Only minions and spells are supported for now.";
        }

        c->initCard(forPlayer);
        deck << c;
    }

    forPlayer->setDeck(deck, false);
}

Hero* CardDB::buildHero(const QString& id)
{
    const CardInfo* c = mCardFlyweights.value(mCardIDsTranslationTable.value(id), NULL);
    if (c == NULL)
        qCritical() << "Hero with id" << id << "is not found in database.";

    if (c->type() != CardTypes::CARD_HERO)
        qCritical() << "ID" << id << "does not refer to a hero.";

    return new Hero((HeroInfo*) c, NULL);
}

int CardDB::maxCardId() const
{
    return mNextCardID - 1;
}

void CardDB::mInitTextIDsTranslationTable()
{
    foreach (CharacterType a, CharacterTypes::availableTypes()) {
        mTextIDsTranslationTable.insert(CharacterTypes::toString(a), a);
    }

    foreach (CardType a, CardTypes::availableTypes()) {
        mTextIDsTranslationTable.insert(CardTypes::toString(a), a);
    }

    foreach (Owner o, Owners::availableOwners()) {
        mTextIDsTranslationTable.insert(Owners::toString(o), o);
    }

    foreach (Ability a, Abilities::availableAbilities()) {
        mTextIDsTranslationTable.insert(Abilities::toString(a), a);
    }

    foreach (ActionType a, ActionTypes::availableTypes()) {
        mTextIDsTranslationTable.insert(ActionTypes::toString(a), a);
    }

    mTextIDsTranslationTable.insert("TARGET_ALLY_HERO", ALLY_HERO);
    mTextIDsTranslationTable.insert("TARGET_BOTH_HEROES", BOTH_HEROES);
    mTextIDsTranslationTable.insert("TARGET_ENEMY_HERO", ENEMY_HERO);
    mTextIDsTranslationTable.insert("SELECT_N_IN_GROUP", SELECT_N_IN_GROUP);
    mTextIDsTranslationTable.insert("PICK_N_FROM_GROUP", PICK_N_FROM_GROUP);
    mTextIDsTranslationTable.insert("TARGET_GROUP", TARGET_GROUP);
    mTextIDsTranslationTable.insert("TARGET_THIS", TARGET_THIS);
    mTextIDsTranslationTable.insert("EVENT_CHARACTER_PARAMS", EVENT_CHARACTER_PARAMS);

    mTextIDsTranslationTable.insert("ATTACKED_CHARACTER", ATTACKED_CHARACTER);
    mTextIDsTranslationTable.insert("ATTACKING_CHARACTER", ATTACKING_CHARACTER);
    mTextIDsTranslationTable.insert("CARD_PLAYED", CARD_PLAYED);
    mTextIDsTranslationTable.insert("DAMAGED_CHARACTER", DAMAGED_CHARACTER);
    mTextIDsTranslationTable.insert("DYING_MINION", DYING_MINION);
    mTextIDsTranslationTable.insert("HEALED_CHARACTER", HEALED_CHARACTER);
    mTextIDsTranslationTable.insert("HEALER", HEALER);
    mTextIDsTranslationTable.insert("PLAYER_ENDING_TURN", PLAYER_ENDING_TURN);

    mTextIDsTranslationTable.insert("AFTER_CARD_PLAYED", Event::AFTER_CARD_PLAYED);
    mTextIDsTranslationTable.insert("BEFORE_CARD_PLAYED", Event::BEFORE_CARD_PLAYED);
    mTextIDsTranslationTable.insert("CHARACTER_ATTACKS", Event::CHARACTER_ATTACKS);
    mTextIDsTranslationTable.insert("CHARACTER_DAMAGED", Event::CHARACTER_DAMAGED);
    mTextIDsTranslationTable.insert("CHARACTER_HEALED", Event::CHARACTER_HEALED);
    mTextIDsTranslationTable.insert("MINION_DIES", Event::MINION_DIES);

    mTextIDsTranslationTable.insert("DMG_SOURCE_ANY", CharacterDamagedTrigger::ANY);
    mTextIDsTranslationTable.insert("DMG_SOURCE_CHARACTER", CharacterDamagedTrigger::CHARACTER);
    mTextIDsTranslationTable.insert("DMG_SOURCE_HERO", CharacterDamagedTrigger::HERO);
    mTextIDsTranslationTable.insert("DMG_SOURCE_MINION", CharacterDamagedTrigger::MINION);
    mTextIDsTranslationTable.insert("DMG_SOURCE_SPELL", CharacterDamagedTrigger::SPELL);
}


CardDB::CardDB():
    mNextActionID(1), // 1 because 0 is reserved for all AttackActions
    mNextCardID(2) // 0 reserved for THE_COIN and 1 for EXCESS_MANA
{ }

TargetExpression* CardDB::mParseTarget(const QJsonObject& target, Event::Type eType)
{
    if (target.isEmpty())
        return new TargetsFromGroupExpression(
                    PickingTypes::USER_SELECT,
                    target.value("quantity").toInt(1),
                    mParseGroup(target.value("group").toObject(), eType));

    switch (target.value("id").toInt(-1)) {
        case ALLY_HERO:
            return new TargetsFromGroupExpression(
                        PickingTypes::RANDOM,
                        target.value("quantity").toInt(1),
                        GroupExpression(Owners::ALLY, CharacterTypes::HERO, QSharedPointer<TargetExpression>(NULL)));

        case BOTH_HEROES:
            return new TargetsFromGroupExpression(
                        PickingTypes::ALL,
                        0,
                        GroupExpression(Owners::ANY_OWNER, CharacterTypes::HERO, QSharedPointer<TargetExpression>(NULL)));

        case ENEMY_HERO:
            return new TargetsFromGroupExpression(
                        PickingTypes::RANDOM,
                        target.value("quantity").toInt(1),
                        GroupExpression(Owners::ENEMY, CharacterTypes::HERO, QSharedPointer<TargetExpression>(NULL)));

        case SELECT_N_IN_GROUP:
            return new TargetsFromGroupExpression(
                        PickingTypes::USER_SELECT,
                        target.value("quantity").toInt(1),
                        mParseGroup(target.value("group").toObject(), eType),
                        target.value("withReplacement").toBool(false));

        case PICK_N_FROM_GROUP:
            return new TargetsFromGroupExpression(
                        PickingTypes::RANDOM,
                        target.value("quantity").toInt(1),
                        mParseGroup(target.value("group").toObject(), eType),
                        target.value("withReplacement").toBool(false));

        case TARGET_GROUP:
            return new TargetsFromGroupExpression(
                        PickingTypes::ALL,
                        0,
                        mParseGroup(target.value("group").toObject(), eType));

        case TARGET_THIS:
            return new TargetThisExpression();

        case EVENT_CHARACTER_PARAMS: {
            QVector<EventParam> params;
            Q_ASSERT(target.contains("params"));
            if (target.value("params").isArray()) {
                foreach (QJsonValue v, target.value("params").toArray())
                    params.append(mIdToEventParam((EventParamType) v.toInt(-1)));
            }
            else {
                params.append(mIdToEventParam((EventParamType) target.value("params").toInt(-1)));
            }

            return new EventCharacterParamExpression(eType, params);
        }

        default:
            qCritical() << "Unrecognized id" << target.value("id") << "for target: " << target;
            return NULL;
    }
}

GroupExpression CardDB::mParseGroup(const QJsonObject& group, Event::Type eType)
{
    if (group.isEmpty())
        return GroupExpression(Owners::ANY_OWNER, CharacterTypes::CHARACTER, QSharedPointer<TargetExpression>(NULL));

    TargetExpression* excludedTargets = NULL;
    if (group.value("excluded").isObject())
        excludedTargets = mParseTarget(group.value("excluded").toObject(), eType);

    return GroupExpression(
                group.value("owner").toInt(Owners::ANY_OWNER),
                group.value("subtype").toInt(CharacterTypes::CHARACTER),
                QSharedPointer<TargetExpression>(excludedTargets));
}

Action* CardDB::mParseAction(const QJsonObject& action, Event::Type eType)
{
    using namespace ActionTypes;
    switch (action.value("id").toInt(-1)) {
        case ADD_ARMOR:
            return new AddArmorAction(
                        action.value("quantity").toInt(1),
                        action.value("target").toInt(Owners::ALLY));

        case ADD_MANA:
            return new AddManaAction(
                        action.value("target").toInt(Owners::ALLY),
                        action.value("quantity").toInt(1),
                        action.value("permanent").toBool(false));

        case DEAL_DMG:
            return new DealDmgHealingAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)),
                        action.value("quantity").toInt(1));

        case DRAW_CARD:
            return new DrawCardAction(
                        action.value("quantity").toInt(1),
                        action.value("targetOwner").toInt(Owners::ALLY));

        case ENCHANT_CHARACTER:
            return new EnchantAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)),
                        QSharedPointer<Enchantment>(mParseEnchantment(action.value("enchantment").toObject())));

        case FREEZE_AFTER_DMG:
            return new DamageAndFreezeAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)),
                        action.value("quantity").toInt(1));

        case FREEZE_AFTER_HEAL:
            return new DamageAndFreezeAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)),
                        -action.value("quantity").toInt(1));

        case HEAL:
            return new DealDmgHealingAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)),
                        -action.value("quantity").toInt(1));

        case SET_FROZEN:
            return new FreezeAction(
                        mNextActionID++,
                        QSharedPointer<TargetExpression>(mParseTarget(action.value("target").toObject(), eType)));

        case SILENCE_TARGET: {
            QSharedPointer<TargetExpression> target(mParseTarget(action.value("target").toObject(), eType));
            SilenceTargetAction* sa = new SilenceTargetAction(mNextActionID++, target);
            if (target->canBeHero())
                qWarning() << "Action" << *sa << ": Casting target to MINION is uncertain; it is possibly a HERO. If the cast fails at run time, the program will crash.";
            return sa;
        }

        case SUMMON_MINIONS:
            Q_ASSERT(mCardIDsTranslationTable.contains(action.value("minion").toString("NULL")));
            return new SummonNewMinionsAction(
                       mCardIDsTranslationTable.value(action.value("minion").toString()),
                       action.value("quantity").toInt(1),
                       action.value("targetOwner").toInt(Owners::ALLY));

        default:
            qCritical() << "Unrecognized id" << action.value("id") << "for action: " << action;
            return NULL;
    }
}

Enchantment* CardDB::mParseEnchantment(const QJsonObject& e)
{
    QHash<Event::Type, QVector<Trigger*>* > triggerPowers;
    QJsonValue tps = e.value("triggerPowers");
    if (!tps.isUndefined()) {
        if (tps.isArray()) {
            foreach (QJsonValue v2, tps.toArray()) {
                mParseTriggerPower(v2.toObject(), &triggerPowers);
            }
        }
        else
            mParseTriggerPower(tps.toObject(), &triggerPowers);
    }

    return new Enchantment(
            e.value("temporary").toBool(false),
            e.value("name").toString("invalid_string"),
            e.value("text").toString("invalid_string"),
            e.value("abilities").toInt(Abilities::NO_ABILITY),
            triggerPowers,
            e.value("maxHPModifier").toInt(0),
            e.value("atkModifier").toInt(0),
            e.value("costModifier").toInt(0),
            0);
}

CharacterType CardDB::mParseMinionType(const QJsonValue& ct)
{
    CharacterType type = 0;
    if (ct.isArray()) {
        foreach (QJsonValue v, ct.toArray())
            type |= v.toInt(0);
    }
    else
        type = ct.toInt(CharacterTypes::GENERAL);

    return type;
}

CharacterType CardDB::mParseTargetType(const QJsonValue& ct)
{
    CharacterType type = 0;
    if (ct.isArray()) {
        foreach (QJsonValue v, ct.toArray())
            type |= v.toInt(0);
    }
    else
        type = ct.toInt(CharacterTypes::CHARACTER);

    return type;
}

SpecialPowerAction* CardDB::mParseSpecialPower(const QJsonObject& sp)
{
    return new SpecialPowerAction(mNextCardID++,
                                  sp.value("name").toString("invalid_string"),
                                  sp.value("text").toString("invalid_string"),
                                  sp.value("cost").toInt(2),
                                  mParseActionList(sp.value("actions")));
}

QVector<Action*>* CardDB::mParseActionList(const QJsonValue& actions, Event::Type eType)
{
    QVector<Action*>* as = new QVector<Action*>();
    if (actions.isArray()) {
        foreach (QJsonValue v, actions.toArray())
            as->append(mParseAction(v.toObject(), eType));

        if (as->empty())
            qCritical() << "Action list" << actions << "cannot be empty";
    }
    else {
        as->append(mParseAction(actions.toObject(), eType));
    }

    return as;
}

void CardDB::mParseTriggerPower(const QJsonObject& trgPower,
                                 QHash<Event::Type, QVector<Trigger*>* >* powerTable)
{
    QJsonValue actions = trgPower.value("actions");
    if (actions.isUndefined())
        qCritical() << "No actions for power" << trgPower;

    // TODO: Normalize event syntax
    Event::Type event;
    QJsonObject eventObj = trgPower.value("event").toObject();
    if (eventObj.isEmpty() || eventObj.value("id").toInt(-1) == -1) {
        event = (Event::Type) trgPower.value("event").toInt(-1);
        if (event == -1)
            qCritical() << "Trigger power" << trgPower << "has no valid 'event' field.";
    }
    else
        event = (Event::Type) eventObj.value("id").toInt();

    QVector<Action*>* as = mParseActionList(actions, event);
    Trigger* t = NULL;
    switch (event) {
        case Event::AT_TURN_END:
            t = new TurnEndTrigger(
                     as,
                     trgPower.value("triggeringPlayer").toInt(Owners::ANY_OWNER));
            break;

        case Event::AFTER_CARD_PLAYED:
        case Event::BEFORE_CARD_PLAYED:
            t = new CardPlayedTrigger(
                     as,
                     trgPower.value("cardType").toInt(CardTypes::CARD_MINION),
                     trgPower.value("characterType").toInt(CharacterTypes::MINION),
                     trgPower.value("owningPlayer").toInt(Owners::ANY_OWNER));
            break;

        case Event::CHARACTER_DAMAGED: {
            TargetExpression* damagers = NULL;
            if (eventObj.value("damagers").isObject())
                damagers = mParseTarget(eventObj.value("damagers").toObject(), Event::CHARACTER_DAMAGED);

            TargetExpression* damagedCharacters = NULL;
            if (eventObj.value("damagedCharacters").isObject())
                damagedCharacters = mParseTarget(eventObj.value("damagedCharacters").toObject(), Event::CHARACTER_DAMAGED);

            t = new CharacterDamagedTrigger(as, damagers, damagedCharacters,
                                            (CharacterDamagedTrigger::SourceType) eventObj.value("acceptedSources").toInt(CharacterDamagedTrigger::ANY));
            break;
        }

        case Event::MINION_DIES:
            t = new MinionKilledTrigger(
                     as,
                     trgPower.value("owningPlayer").toInt(Owners::ANY_OWNER),
                     trgPower.value("minionType").toInt(CharacterTypes::MINION));
            break;

        case Event::CHARACTER_ATTACKS:
        case Event::CHARACTER_HEALED:
        default:
            qCritical() << "Event type" << event << "is not supported";
    }

    if (powerTable->contains(event))
        powerTable->value(event)->append(t);
    else
        powerTable->insert(event, new QVector<Trigger*>(1, t));
}

EventParam CardDB::mIdToEventParam(CardDB::EventParamType ep)
{
    switch (ep) {
        case ATTACKED_CHARACTER:
            return EventParams::ATTACKED_CHARACTER;

        case ATTACKING_CHARACTER:
            return EventParams::ATTACKING_CHARACTER;

        case CARD_PLAYED:
            return EventParams::CARD_PLAYED;

        case DAMAGED_CHARACTER:
            return EventParams::DAMAGED_CHARACTER;

        case DYING_MINION:
            return EventParams::DYING_MINION;

        case HEALED_CHARACTER:
            return EventParams::HEALED_CHARACTER;

        case HEALER:
            return EventParams::HEALER;

        case PLAYER_ENDING_TURN:
            return EventParams::PLAYER_ENDING_TURN;

        default:
            qCritical() << "Unrecognized EventParamType";
            return NULL;
    }
}
