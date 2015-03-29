#include "playerinput.h"
#include "iorequest.h"

PlayerInput::PlayerInput(QObject *parent) : QObject(parent)
{

}

PlayerInput::~PlayerInput()
{

}

void PlayerInput::onActionChosen(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onArmorAdded(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onAttack(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onCharacterDamagedOrHealed(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onCardDrawn(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onCardEnchanted(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onDeathRattle(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onBattlecry(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onDrawnCardDestroyed(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onCardPutInHand(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onCardRemovedFromHand(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onGameEnded(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onManaUpdate(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onMinionDestroyed(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onMinionSummoned(IORequest* ir)
{
    ir->clearRef();
}

void PlayerInput::onTargetChosen(IORequest* ir)
{
    ir->clearRef();
}

