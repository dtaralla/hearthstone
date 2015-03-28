#ifndef PLAYERINPUT_H
#define PLAYERINPUT_H

#include <QObject>

class IORequest;
class Player;

class PlayerInput : public QObject
{
    Q_OBJECT

public:
    explicit PlayerInput(QObject *parent = 0);
    virtual ~PlayerInput();

    virtual void attachPlayer(Player* p) = 0;


signals:

public slots:
// Asynchronous slots (they should destroy ir at the end)
    /**
     * @brief onActionChosen
     * Called whenever a player choose an action to perform.
     *
     * @param ir A request IORequest::CHOOSE_ACTION, with extras:
     *     - Action* "action": the action chosen
     *     - Player* "chooser": the player who chose the action
     */
    virtual void onActionChosen(IORequest* ir);

    /**
     * @brief onArmorAdded
     * Called whenever a hero sees his armor modified (but when it comes from
     * an attack; see onAttack for this).
     *
     * @param ir A request IORequest::ADD_ARMOR, with extras:
     *     - Hero* "hero": The hero whose armor has been modified
     */
    virtual void onArmorAdded(IORequest* ir);

    /**
     * @brief onAttack
     * Called whenever an attack between two characters has occured.
     * The owner of the "attacker" is always the player triggering this event
     * (ir->source()).
     *
     * @param ir A request IORequest::ATTACK, with extras:
     *     - Character* "attacker"
     *     - Character* "target"
     */
    virtual void onAttack(IORequest* ir);

    /**
     * @brief onBattlecry
     * Called before any battlecry occurs.
     *
     * @param ir A request IORequest::BATTLECRY, with extras:
     *     - Minion* "minion": the minion issuing the battlecry
     *     - Action* "action": the battlecry action
     */
    virtual void onBattlecry(IORequest* ir);

    /**
     * @brief onCardDrawn
     * Called whenever drawing a card, before it being put in player's hand or
     * discared to player's graveyard.
     * This is also called if the deck was empty and a fatigue card was created.
     *
     * @param ir A request IORequest::DRAW_CARD, with extras:
     *     - Card* "card": A pointer to the drawn card
     *        OR a request IORequest::DRAW_FATIGUE, with extras:
     *     - Player* "player": The player receiving the fatigue card
     */
    virtual void onCardDrawn(IORequest* ir);

    /**
     * @brief onCardEnchanted
     * Called whenever a card is enchanted (or silenced).
     *
     * @param ir A request IORequest::ENCHANT_CHARACTER, with extras:
     *     - Card* "target"
     */
    virtual void onCardEnchanted(IORequest* ir);

    /**
     * @brief onCharacterDamagedOrHealed
     * Called whenever a character takes damage or is healed (excluding
     * when this damage is dealt because of an attack; see onAttack for this)
     *
     * @param ir A request IORequest::DAMAGE_CHARACTER if the character was
     * damaged, a request IORequest::HEAL_CHARACTER if it was healed. In all
     * cases, with extras:
     *     - Character* "target"
     */
    virtual void onCharacterDamagedOrHealed(IORequest* ir);

    /**
     * @brief onDeathRattle
     * Called before any deathrattle effect actually occurs.
     *
     * @param ir A request IORequest::DEATHRATTLE, with extras:
     *     - Character* "character": the character issuing the death rattle
     *     - Action* "action": the death rattle action
     */
    virtual void onDeathRattle(IORequest* ir);

    /**
     * @brief onDrawnCardDestroyed
     * Called on drawing a card while the player's hand already contains 10
     * cards. This can also occurs if the card was PUT_IN_HAND.
     *
     * @param ir A request IORequest::DESTROY_CARD, with extras:
     *     - Card* "card": A pointer to the card to destroy
     */
    virtual void onDrawnCardDestroyed(IORequest* ir);

    /**
     * @brief onCardPutInHand
     * Called whenever a player sees a card being put in his hand.
     *
     * @param ir A request IORequest::PUT_CARD_IN_HAND, with extras:
     *     - Card* "card": A pointer to the card put in hand
     */
    virtual void onCardPutInHand(IORequest* ir);

    /**
     * @brief onCardRemovedFromHand
     * Called whenever a player sees a card removed from his hand.
     *
     * @param ir A request IORequest::REMOVE_FROM_HAND, with extras:
     *     - Card* "card": A pointer to the card removed
     */
    virtual void onCardRemovedFromHand(IORequest* ir);

    /**
     * @brief onGameEnded
     * Called when the results of the game are known.
     *
     * @param ir A request IORequest::LOST, WON, TIE according to the result
     */
    virtual void onGameEnded(IORequest* ir);

    /**
     * @brief onManaUpdate
     * Called whenever the mana reserve status is updated.
     *
     * @param ir A request IORequest::MANA_UPDATE, with extras:
     *     - Player* "target": The player whose mana is updated
     *     - int "maxMana": Quantity of mana cristals
     *     - int "curMana": Currently available mana
     */
    virtual void onManaUpdate(IORequest* ir);

    /**
     * @brief onMinionDestroyed
     * Called whenever a minion has to be put into the graveyard.
     *
     * @param ir A request IORequest::DESTROY_MINION, with extras:
     *     - Minion* "minion": The minion to put into the graveyard
     */
    virtual void onMinionDestroyed(IORequest* ir);

    /**
     * @brief onMinionSummoned
     * Called whenever a minion is summoned on the board.
     *
     * @param ir A request IORequest::SUMMON_MINION, with extras:
     *     - Minion* "minion"
     *     - int "position": between 0 and owner->minions()->size() included
     */
    virtual void onMinionSummoned(IORequest* ir);

    /**
     * @brief onTargetChosen
     * Called whenever a player choose a target for an action.
     *
     * @param ir A request IORequest::CHOOSE_TARGET, with extras:
     *     - Character* "target": the target chosen
     *     - Player* "targetter": the player who chose the target
     */
    virtual void onTargetChosen(IORequest* ir);


// Synchronous slots (they should set ir response at the end)
    /**
     * @brief onEventTriggered
     * Called whenever something is triggered (not each time an event occurs;
     * only when there is something to trigger).
     * The response set by this slot will be ignored by the signalling context;
     * it should not set any else a memory leak could occur.
     * NB: We need this slot to be synchronous, because "event" and "action"
     *     extras might be destroyed before these slots are executed!
     *
     * @param ir A request IORequest::TRIGGER, with extras:
     *     - Event* "event": The event that occured.
     *     - Action* "action": The action which is triggered
     */
    virtual void onEventTriggered(IORequest* ir) = 0;

    /**
     * @brief askForAction
     * Called whenever the game needs an input for choosing an actions among a
     * predefined set (the "availableActions" extra).
     *
     * @param ir A request IORequest::ACTION_SELECTION, with extras:
     *     - const QVector<Action*>* "availableActions"
     */
    virtual void askForAction(IORequest* ir) = 0;

    /**
     * @brief askForPosition
     * Called whenever the game needs an input for choosing a position between
     * a player's minions. This usually arrives when playing a minion (the
     * player has then to select WHERE among his minions it will be summoned).
     *
     * @param ir A request InputReques::POSITION_SELECTION, with extras:
     *     - const Minion* "minion": the minion that need to be placed
     */
    virtual void askForPosition(IORequest* ir) = 0;

    /**
     * @brief askForTarget
     * Called whenever the game needs an input for choosing a target.
     * Should set the response field of ir with a pointer to the selected
     * character (chosen from "availableTargets" extra).
     *
     * @param ir A request IORequest::TARGET_SELECTION, with extras:
     *     - const QVector<Character*>* "availableTargets"
     *     - Action* "action": The action that needs selection
     */
    virtual void askForTarget(IORequest* ir) = 0;
};

#endif // PLAYERINPUT_H
