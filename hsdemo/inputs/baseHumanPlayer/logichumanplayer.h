#ifndef LOGICHUMANPLAYER_H
#define LOGICHUMANPLAYER_H

#include "playerinput.h"
#include <QDebug>

class Player;
class Hero;
class GUIHumanPlayer;

class LogicHumanPlayer : public PlayerInput
{
    Q_OBJECT

public:
    LogicHumanPlayer();
    virtual ~LogicHumanPlayer();

    virtual void attachPlayer(Player* p);

protected:
    Player* m_player;
    GUIHumanPlayer* m_gui;
    QString m_logString;
    QDebug m_log;

    void m_flushLog();

    // PlayerInput interface
public slots:
    void onActionChosen(IORequest* ir);
    void onArmorAdded(IORequest* ir);
    void onAttack(IORequest* ir);
    void onBattlecry(IORequest* ir);
    void onCharacterDamagedOrHealed(IORequest* ir);
    void onCardEnchanted(IORequest* ir);
    void onCardDrawn(IORequest* ir);
    void onCardPutInHand(IORequest* ir);
    void onCardRemovedFromHand(IORequest* ir);
    void onDeathRattle(IORequest* ir);
    void onDrawnCardDestroyed(IORequest* ir);
    void onGameEnded(IORequest* ir);
    void onManaUpdate(IORequest* ir);
    void onMinionDestroyed(IORequest* ir);
    void onMinionSummoned(IORequest* ir);
    void onTargetChosen(IORequest* ir);

    void onEventTriggered(IORequest* ir);
    virtual void askForAction(IORequest* ir);
    virtual void askForPosition(IORequest* ir);
    virtual void askForTarget(IORequest* ir);

};

#endif // LOGICHUMANPLAYER_H
