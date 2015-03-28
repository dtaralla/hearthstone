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
    ir->deleteLater();
}

void PlayerInput::onArmorAdded(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onAttack(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onCharacterDamagedOrHealed(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onCardDrawn(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onCardEnchanted(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onDeathRattle(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onBattlecry(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onDrawnCardDestroyed(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onCardPutInHand(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onCardRemovedFromHand(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onGameEnded(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onManaUpdate(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onMinionDestroyed(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onMinionSummoned(IORequest* ir)
{
    ir->deleteLater();
}

void PlayerInput::onTargetChosen(IORequest* ir)
{
    ir->deleteLater();
}

