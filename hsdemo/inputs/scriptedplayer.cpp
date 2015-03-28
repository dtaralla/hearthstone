#include "scriptedplayer.h"
#include "game.h"
#include "iorequest.h"
#include "actions/allactions.h"
#include "card.h"
#include "minion.h"
#include "spell.h"
#include "player.h"
#include "enchantment.h"

#include "expressions/target/targetexpression.h"
#include "expressions/target/targetsfromgroupexpression.h"

ScriptedPlayer::ScriptedPlayer(QObject* parent) :
    PlayerInput(parent),
    m_me(NULL),
    m_enemy(NULL)
{

}

ScriptedPlayer::~ScriptedPlayer()
{

}

void ScriptedPlayer::attachPlayer(Player* p)
{
    m_me = p;
    m_enemy = p->opponent();
}

int ScriptedPlayer::m_cardScore(Card* c)
{
    if (c->type() == CardTypes::CARD_MINION) {
        Minion* m = (Minion*) c;
        return m->atk() + m->hp();
    }

    Q_ASSERT(c->type() == CardTypes::CARD_SPELL);

    int score = 0;
    foreach (Action* a, *((Spell*) c)->playActionConsequences()) {
        score += m_actionScore(a);
    }

    return score;
}

int ScriptedPlayer::m_actionScore(Action* a)
{
    int score = 0;
    switch (a->type()) {
        case ActionTypes::ADD_ARMOR: {
            AddArmorAction* trueAction = (AddArmorAction*) a;
            if (trueAction->targetOwner() == Owners::ANY_OWNER)
                score = 0;
            else if (trueAction->targetOwner() == Owners::ALLY)
                score = trueAction->quantity();
            else
                score = -trueAction->quantity();
            break;
        }




        case ActionTypes::ADD_MANA: {
            AddManaAction* trueAction = (AddManaAction*) a;

            if (trueAction->targetOwner() == Owners::ANY_OWNER)
                score = 0;
            else if (trueAction->targetOwner() == Owners::ALLY)
                score = trueAction->quantity();
            else
                score = -trueAction->quantity();
            break;
        }




        case ActionTypes::FREEZE_AFTER_DMG: {
            DamageAndFreezeAction* trueAction = (DamageAndFreezeAction*) a;
            // We do not take into account ownership difficult to evaluate

            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }
            else if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score += m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  trueAction->quantity() * enemyCount / (enemyCount + allyCount);
                        score -= trueAction->quantity() * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (enemTargets->empty()) {
                            Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), allyTargets);
                            score = -m_dmgScore(trueAction->quantity(), c) - m_freezeScore(c);
                        }
                        else {
                            Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), enemTargets);
                            score = m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score -= trueAction->quantity() * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), allyTargets);
                        score = -m_dmgScore(trueAction->quantity(), c) - m_freezeScore(c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score += m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score += trueAction->quantity() * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), enemTargets);
                        score = m_dmgScore(trueAction->quantity(), c) + m_freezeScore(c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }


        case ActionTypes::DEAL_DMG: {
            DealDmgHealingAction* trueAction = (DealDmgHealingAction*) a;
            // We do not take into account ownership difficult to evaluate

            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }
            else if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_dmgScore(trueAction->quantity(), c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score += m_dmgScore(trueAction->quantity(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  trueAction->quantity() * enemyCount / (enemyCount + allyCount);
                        score -= trueAction->quantity() * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (enemTargets->empty()) {
                            Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), allyTargets);
                            score = -m_dmgScore(trueAction->quantity(), c);
                        }
                        else {
                            Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), enemTargets);
                            score = m_dmgScore(trueAction->quantity(), c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_dmgScore(trueAction->quantity(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score -= trueAction->quantity() * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), allyTargets);
                        score = -m_dmgScore(trueAction->quantity(), c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score += m_dmgScore(trueAction->quantity(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score += trueAction->quantity() * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestDmgSpellTarget(trueAction->quantity(), enemTargets);
                        score = m_dmgScore(trueAction->quantity(), c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }




        case ActionTypes::ENCHANT_CHARACTER: {
            EnchantAction* trueAction = (EnchantAction*) a;
            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }

            if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_enchantScore(trueAction->enchantment(), c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score += m_enchantScore(trueAction->enchantment(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  -3 * enemyCount / (enemyCount + allyCount);
                        score += 3 * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (enemTargets->empty()) {
                            Character* c = m_selectBestEnchantmentTarget(trueAction->enchantment(), allyTargets);
                            score = -m_enchantScore(trueAction->enchantment(), c);
                        }
                        else {
                            Character* c = m_selectBestFreezingSpellTarget(enemTargets);
                            score = m_enchantScore(trueAction->enchantment(), c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_enchantScore(trueAction->enchantment(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = -3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestEnchantmentTarget(trueAction->enchantment(), allyTargets);
                        score = -m_enchantScore(trueAction->enchantment(), c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score += m_enchantScore(trueAction->enchantment(), c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = 3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestEnchantmentTarget(trueAction->enchantment(), enemTargets);
                        score = m_enchantScore(trueAction->enchantment(), c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }




        case ActionTypes::FREEZE_AFTER_HEAL: {
            DamageAndFreezeAction* trueAction = (DamageAndFreezeAction*) a;
            // We do not take into account ownership difficult to evaluate

            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();
            int healing = -trueAction->quantity();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }
            else if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score += m_healingScore(healing, c) - m_freezeScore(c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score -= m_healingScore(healing, c) - m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  -(healing+1) * enemyCount / (enemyCount + allyCount);
                        score += (healing+1) * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (allyTargets->empty()) {
                            Character* c = m_selectBestHealingSpellTarget(healing, enemTargets);
                            score = -m_healingScore(healing, c) + m_freezeScore(c);
                        }
                        else {
                            Character* c = m_selectBestHealingSpellTarget(healing, allyTargets);
                            score = m_healingScore(healing, c) - m_freezeScore(c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score += m_healingScore(healing, c) - m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = healing * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestHealingSpellTarget(healing, allyTargets);
                        score = m_healingScore(healing, c) - m_freezeScore(c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score -= m_healingScore(healing, c) - m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = -healing * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestHealingSpellTarget(healing, enemTargets);
                        score = -m_healingScore(healing, c) + m_freezeScore(c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }


        case ActionTypes::HEAL: {
            DealDmgHealingAction* trueAction = (DealDmgHealingAction*) a;
            // We do not take into account ownership difficult to evaluate

            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();
            int healing = -trueAction->quantity();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }
            else if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score += m_healingScore(healing, c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score -= m_healingScore(healing, c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  -healing * enemyCount / (enemyCount + allyCount);
                        score += healing * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (allyTargets->empty()) {
                            Character* c = m_selectBestHealingSpellTarget(healing, enemTargets);
                            score = -m_healingScore(healing, c);
                        }
                        else {
                            Character* c = m_selectBestHealingSpellTarget(healing, allyTargets);
                            score = m_healingScore(healing, c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score += m_healingScore(healing, c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = healing * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestHealingSpellTarget(healing, allyTargets);
                        score = m_healingScore(healing, c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score -= m_healingScore(healing, c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = -healing * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestHealingSpellTarget(healing, enemTargets);
                        score = -m_healingScore(healing, c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }




        case ActionTypes::DRAW_CARD: {
            DrawCardAction* trueAction = (DrawCardAction*) a;
            int enemMax = 10 - m_enemy->hand()->size();
            int allyMax = 10 - m_me->hand()->size();
            int enemDraw = trueAction->quantity();
            if (enemDraw > enemMax)
                enemDraw = enemMax;
            int allyDraw = trueAction->quantity();
            if (allyDraw > allyMax)
                allyDraw = allyMax;

            if (trueAction->targetOwner() == Owners::ANY_OWNER)
                score = allyDraw - enemDraw;
            else if (trueAction->targetOwner() == Owners::ALLY)
                score = allyDraw;
            else
                score = -enemDraw;

            score *= 4;
            break;
        }


        case ActionTypes::SILENCE_TARGET: {
            SilenceTargetAction* trueAction = (SilenceTargetAction*) a;
            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }

            if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_silenceScore(c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score += m_silenceScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  -3 * enemyCount / (enemyCount + allyCount);
                        score += 3 * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (enemTargets->empty()) {
                            Character* c = m_selectBestSilencingSpellTarget(allyTargets);
                            score = -m_silenceScore(c);
                        }
                        else {
                            Character* c = m_selectBestSilencingSpellTarget(enemTargets);
                            score = m_silenceScore(c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_silenceScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = -3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestSilencingSpellTarget(allyTargets);
                        score = -m_silenceScore(c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score += m_silenceScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = 3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestSilencingSpellTarget(enemTargets);
                        score = m_silenceScore(c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }

        case ActionTypes::SET_FROZEN: {
            FreezeAction* trueAction = (FreezeAction*) a;
            QVector<Character*>* allyTargets = m_me->allTargets();
            QVector<Character*>* enemTargets = m_enemy->allTargets();

            const TargetsFromGroupExpression* target = dynamic_cast<const TargetsFromGroupExpression*>(trueAction->target());
            if (target == NULL) {
                score = 0; // TODO: do smtg else?
            }

            if (target->owner() == Owners::ANY_OWNER) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_freezeScore(c);
                        }
                        foreach (Character* c, *enemTargets) {
                            score += m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        int allyCount = allyTargets->size();
                        int enemyCount = enemTargets->size();
                        score =  -3 * enemyCount / (enemyCount + allyCount);
                        score += 3 * allyCount / (enemyCount + allyCount);
                        score *= target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        if (enemTargets->empty()) {
                            Character* c = m_selectBestFreezingSpellTarget(allyTargets);
                            score = -m_freezeScore(c);
                        }
                        else {
                            Character* c = m_selectBestFreezingSpellTarget(enemTargets);
                            score = m_freezeScore(c);
                        }
                        break;
                    }
                }
            }
            else if (target->owner() == Owners::ALLY) {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *allyTargets) {
                            score -= m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = -3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestFreezingSpellTarget(allyTargets);
                        score = -m_freezeScore(c);
                        break;
                    }
                }
            }
            else {
                switch (target->pickingType()) {
                    case PickingTypes::ALL:
                        foreach (Character* c, *enemTargets) {
                            score += m_freezeScore(c);
                        }
                        break;

                    case PickingTypes::RANDOM: {
                        score = 3 * target->howManySelections();
                        break;
                    }

                    case PickingTypes::USER_SELECT: {
                        Character* c = m_selectBestFreezingSpellTarget(enemTargets);
                        score = m_freezeScore(c);
                        break;
                    }
                }
            }

            delete allyTargets;
            delete enemTargets;
            break;
        }


        case ActionTypes::SPECIAL_POWER: {
            SpecialPowerAction* trueAction = (SpecialPowerAction*) a;
            foreach (Action* a, *trueAction->effects())
                score += m_actionScore(a);
            break;
        }

        case ActionTypes::SUMMON_MINIONS: {
            SummonNewMinionsAction* trueAction = (SummonNewMinionsAction*) a;
            if (trueAction->targetOwner() == Owners::ANY_OWNER) {
                int maxSumm = 7 - m_me->minions()->size();
                int quantity = trueAction->quantity();
                if (quantity > maxSumm)
                    quantity = maxSumm;
                score = quantity;

                maxSumm = 7 - m_enemy->minions()->size();
                quantity = trueAction->quantity();
                if (quantity > maxSumm)
                    quantity = maxSumm;
                score -= quantity;

                score *= trueAction->base()->atk() + trueAction->base()->hp();
            }
            else if (trueAction->targetOwner() == Owners::ALLY) {
                int maxSumm = 7 - m_me->minions()->size();
                int quantity = trueAction->quantity();
                if (quantity > maxSumm)
                    quantity = maxSumm;
                score = quantity * (trueAction->base()->atk() + trueAction->base()->hp());
            }
            else if (trueAction->targetOwner() == Owners::ENEMY) {
                int maxSumm = 7 - m_enemy->minions()->size();
                int quantity = trueAction->quantity();
                if (quantity > maxSumm)
                    quantity = maxSumm;
                score = -quantity * (trueAction->base()->atk() + trueAction->base()->hp());
            }
            break;
        }

        default:
            qCritical() << "Unrecognized action:" << *a;
    }

    return score;
}

int ScriptedPlayer::m_dmgScore(int dmg, Character* onChar)
{
    if (onChar == NULL)
        return -1;

    if (onChar->type() == CardTypes::CARD_HERO && onChar->hp() <= dmg)
        return 9999999;

    if (onChar->hp() > dmg)
        return dmg;

    return onChar->hp() + onChar->atk();
}

int ScriptedPlayer::m_healingScore(int healing, Character* onChar)
{
    if (onChar == NULL)
        return -1;

    int maxHeal = onChar->maxHp() - onChar->hp();
    if (healing > maxHeal)
        healing = maxHeal;

    return healing;
}

int ScriptedPlayer::m_silenceScore(Character* onChar)
{
    if (onChar == NULL)
        return -1;

    int score = onChar->triggerPowersCount(); // suppose they only grant benefits
    foreach (Ability a, Abilities::availableAbilities()) {
        if (onChar->hasAbility(a))
            score += 1;
    }
    if (onChar->hasAbility(Abilities::FROZEN))
        score -= 2;

    return score;
}

int ScriptedPlayer::m_enchantScore(const Enchantment* e, Character* onChar)
{
    if (onChar == NULL)
        return -1;

    return onChar->atk() + onChar->hp() + e->atkModifier() + e->maxHPModifier();
}

int ScriptedPlayer::m_freezeScore(Character* onChar)
{
    if (onChar == NULL)
        return -1;

    return onChar->atk();
}

int ScriptedPlayer::m_attackScore(Character* atker, Character* c)
{
    int score = 0;
    int targetLifeAfterAtk = c->hp() - atker->atk();
    int atkerLifeAfterAtk = atker->hp() - c->atk();

    if (targetLifeAfterAtk <= 0)
        score += c->hp() + c->atk();
    else
        score += atker->atk();

    if (atkerLifeAfterAtk <= 0)
        score -= atker->hp() + atker->atk();
    else
        score -= c->atk();

    return score;
}

Character* ScriptedPlayer::m_selectBestAttackTarget(Character* atker, QVector<Character*>* targets)
{
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_attackScore(atker, c);

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
        else if (score == bestTargetScore && bestTarget != NULL) {
            if (bestTarget->type() == CardTypes::CARD_HERO)
                bestTarget = c; // Give priority to trade minions rather than atking hero
        }
    }

    return bestTarget;
}

Character* ScriptedPlayer::m_selectBestDmgSpellTarget(int dmg, QVector<Character*>* targets)
{
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_dmgScore(dmg, c);
        if (c->owner() == m_me)
            score = -score;

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
    }

    return bestTarget;
}

Character* ScriptedPlayer::m_selectBestHealingSpellTarget(int heal, QVector<Character*>* targets)
{
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_healingScore(heal, c);
        if (c->owner() != m_me)
            score = -score;

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
    }

    return bestTarget;
}

Character* ScriptedPlayer::m_selectBestSilencingSpellTarget(QVector<Character*>* targets)
{
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_silenceScore(c);

        if (c->owner() == m_me)
            score = -score;

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
    }

    return bestTarget;
}

Character* ScriptedPlayer::m_selectBestFreezingSpellTarget(QVector<Character*>* targets)
{
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_freezeScore(c);

        if (c->owner() == m_me)
            score = -score;

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
    }

    return bestTarget;
}

Character* ScriptedPlayer::m_selectBestEnchantmentTarget(const Enchantment* e, QVector<Character*>* targets)
{
    // Only look for HP/ATK
    Character* bestTarget = NULL;
    int bestTargetScore = -999999999;
    foreach (Character* c, *targets) {
        int score = m_enchantScore(e, c);

        if (c->owner() != m_me)
            score = -score;

        if (score > bestTargetScore) {
            bestTargetScore = score;
            bestTarget = c;
        }
    }

    return bestTarget;
}



void ScriptedPlayer::onEventTriggered(IORequest* ir)
{
    Q_UNUSED(ir);
}

void ScriptedPlayer::askForAction(IORequest* ir)
{
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));

    // First, only consider playActions: always play cards/sp. power if possible
    Action* bestAction = NULL;
    int bestScore = 0;
    foreach (Action* a, *actions) {
        int score = 0;
        if (a->type() == ActionTypes::PLAY_CARD) {
            score = m_cardScore(a->source());
        }
        else if (a->type() == ActionTypes::SPECIAL_POWER) {
            foreach (Action* a, *((SpecialPowerAction*) a)->effects())
                score += m_actionScore(a);
        }
        else
            continue;

        if (bestAction == NULL || score > bestScore) {
            bestAction = a;
            bestScore = score;
        }
    }

    if (bestAction != NULL) {
        ir->setResponse(bestAction);
        return;
    }

    // No play action? Process attacks then
    QVector<Character*>* atkables = m_enemy->attackableCharacters();

    // Select the atker which would lay the best score when selecting its best target
    foreach (Action* a, *actions) {
        if (a->type() == ActionTypes::END_TURN && bestAction == NULL) {
            bestAction = a;
        }
        if (a->type() != ActionTypes::ATTACK)
            continue;

        // This is an attack action
        Character* atker = (Character*) a->source();

        Character* bestTarget = m_selectBestAttackTarget(atker, atkables);
        int bestTargetScore = m_attackScore(atker, bestTarget);

        if (bestTargetScore > bestScore) {
            bestScore = bestTargetScore;
            bestAction = a;
        }

    }
    delete atkables;

    // If no attack was giving a sufficient score (> 0), bestAction is automatically EndTurn
    ir->setResponse(bestAction);
}

void ScriptedPlayer::askForPosition(IORequest* ir)
{
    ir->setResponse(new int(0));
}

void ScriptedPlayer::askForTarget(IORequest* ir)
{
    Action* a = VPtr<Action>::AsPtr(ir->extra("action"));
    QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));

    switch (a->type()) {
        case ActionTypes::ATTACK:
            ir->setResponse(m_selectBestAttackTarget((Character*) a->source(), targets));
            break;

        case ActionTypes::FREEZE_AFTER_DMG:
        case ActionTypes::DEAL_DMG: {
            DealDmgHealingAction* trueAction = (DealDmgHealingAction*) a;
            ir->setResponse(m_selectBestDmgSpellTarget(trueAction->quantity(), targets));
            break;
        }

        case ActionTypes::ENCHANT_CHARACTER: {
            EnchantAction* trueAction = (EnchantAction*) a;
            ir->setResponse(m_selectBestEnchantmentTarget(trueAction->enchantment(), targets));
            break;

        }

        case ActionTypes::FREEZE_AFTER_HEAL:
        case ActionTypes::HEAL: {
            DealDmgHealingAction* trueAction = (DealDmgHealingAction*) a;
            ir->setResponse(m_selectBestHealingSpellTarget(trueAction->quantity(), targets));
            break;
        }

        case ActionTypes::SILENCE_TARGET: {
            ir->setResponse(m_selectBestSilencingSpellTarget(targets));
            break;
        }

        case ActionTypes::SET_FROZEN: {
            ir->setResponse(m_selectBestFreezingSpellTarget(targets));
            break;
        }

        default:
            qCritical() << "The following action is not a targetted action:" << *a;
    }
}
