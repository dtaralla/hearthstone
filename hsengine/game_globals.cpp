#include "game_globals.h"
#include "player.h"
#include "minion.h"

QVector<Owner> Owners::availableOwners() {
    using namespace Owners;
    QVector<Owner> ows;
    ows << ANY_OWNER
        << ALLY
        << ENEMY;

    return ows;
}

QString Owners::toString(Owner o)
{
    using namespace Owners;
    switch(o) {
        case ANY_OWNER:
            return "OWNER_ANY";
        case ALLY:
            return "OWNER_ALLY";
        case ENEMY:
            return "OWNER_ENEMY";
        default:
            return "UNRECOGNIZED_OWNER";
    }
}

QVector<CharacterType> CharacterTypes::availableTypes() {
    using namespace CharacterTypes;
    QVector<CharacterType> types;
    types << UNDEAD
          << BEAST
          << DEMON
          << DRAGON
          << GENERAL
          << MECH
          << MURLOC
          << PIRATE
          << TOTEM
          << MINION
          << HERO
          << CHARACTER;

    return types;
}

QString CharacterTypes::toString(CharacterType ct)
{
    using namespace CharacterTypes;
    switch(ct) {
        case UNDEAD:
            return "TYPE_UNDEAD";
        case BEAST:
            return "TYPE_BEAST";
        case DEMON:
            return "TYPE_DEMON";
        case DRAGON:
            return "TYPE_DRAGON";
        case GENERAL:
            return "TYPE_GENERAL";
        case MECH:
            return "TYPE_MECH";
        case MURLOC:
            return "TYPE_MURLOC";
        case PIRATE:
            return "TYPE_PIRATE";
        case TOTEM:
            return "TYPE_TOTEM";
        case MINION:
            return "TYPE_MINION";
        case HERO:
            return "HERO";
        case CHARACTER:
            return "TYPE_CHARACTER";
        default:
            return "UNRECOGNIZED_CHARACTERTYPE";
    }
}

QVector<CardType> CardTypes::availableTypes() {
    using namespace CardTypes;
    QVector<CardType> types;
    types << CARD_CHARACTER
          << CARD_HERO
          << CARD_MINION
          << CARD_SECRET
          << CARD_SPELL
          << CARD_WEAPON;

    return types;
}

QString CardTypes::toString(CardType ct)
{
    using namespace CardTypes;
    switch(ct) {
        case CARD_MINION:
            return "CARD_MINION";
        case CARD_HERO:
            return "CARD_HERO";
        case CARD_CHARACTER:
            return "CARD_CHARACTER";
        case CARD_SPELL:
             return "CARD_SPELL";
        case CARD_SECRET:
            return "CARD_SECRET";
        case CARD_WEAPON:
            return "CARD_WEAPON";
        default:
            return "UNRECOGNIZED_CARDTYPE";
    }
}

QVector<Ability> Abilities::availableAbilities() {
    using namespace Abilities;
    QVector<Ability> abs;
    abs << CHARGE
        << TAUNT;

    return abs;
}

QString Abilities::toString(Ability a)
{
    using namespace Abilities;
    switch(a) {
        case CHARGE:
            return "ABILITY_CHARGE";
        case TAUNT:
            return "ABILITY_TAUNT";
        case FROZEN:
            return "ABILITY_FROZEN";
        default:
            return "UNRECOGNIZED_ABILITY";
    }
}

QVector<ActionType> ActionTypes::availableTypes() {
    using namespace ActionTypes;
    QVector<ActionType> types;
    types << ADD_ARMOR
          << ADD_MANA
          << DEAL_DMG
          << DRAW_CARD
          << ENCHANT_CHARACTER
          << FREEZE_AFTER_DMG
          << FREEZE_AFTER_HEAL
          << HEAL
          << SET_FROZEN
          << SILENCE_TARGET
          << SUMMON_MINIONS;

    return types;
}


QString ActionTypes::toString(ActionTypes::ActionType a)
{
    using namespace ActionTypes;
    switch(a) {
        case ADD_ARMOR:
            return "ADD_ARMOR";

        case ADD_MANA:
            return "ADD_MANA";

        case DEAL_DMG:
            return "DEAL_DMG";

        case DRAW_CARD:
            return "DRAW_CARD";

        case ENCHANT_CHARACTER:
            return "ENCHANT_CHARACTER";

        case FREEZE_AFTER_DMG:
            return "FREEZE_AFTER_DMG";

        case FREEZE_AFTER_HEAL:
            return "FREEZE_AFTER_HEAL";

        case HEAL:
            return "HEAL";

        case SET_FROZEN:
            return "SET_FROZEN";

        case SILENCE_TARGET:
            return "SILENCE_TARGET";

        case SUMMON_MINIONS:
            return "SUMMON_MINIONS";

        default:
            return "UNRECOGNIZED_ABILITY";
    }
}
