#include "logichumanplayer.h"
#include "guihumanplayer.h"
#include "ui_guihumanplayer.h"
#include "logwindow.h"
#include "selectactiontargetdialog.h"
#include "selectpositiondialog.h"
#include "card.h"
#include "player.h"
#include "game.h"
#include "hero.h"
#include "minion.h"
#include "actions/specialpoweraction.h"
#include "enchantment.h"
#include "iorequest.h"
#include "vptr.h"
#include <QtDebug>

LogicHumanPlayer::LogicHumanPlayer() :
    PlayerInput(),
    m_log(&m_logString)
{
    m_log.noquote();
    m_gui = new GUIHumanPlayer();
    m_gui->show();
}

LogicHumanPlayer::~LogicHumanPlayer()
{
    delete m_gui;
}

void LogicHumanPlayer::attachPlayer(Player* p)
{
    m_player = p;
    m_gui->setWindowTitle(p->name() % " (" % p->hero()->base()->name() % ")");
    m_gui->logWindow()->setWindowTitle(m_gui->windowTitle() % " log");

    SpecialPowerAction* sp = (SpecialPowerAction*) m_player->hero()->specialPower();
    m_gui->initSpecialPower(sp, true);

    Player* oppositePlayer = m_player->opponent();
    sp = (SpecialPowerAction*) oppositePlayer->hero()->specialPower();
    m_gui->initSpecialPower(sp, false);
}

void LogicHumanPlayer::m_flushLog()
{
    m_gui->addEntryToLog(m_logString);
    m_logString = "";
}

void LogicHumanPlayer::onActionChosen(IORequest* ir)
{
    Action* a = VPtr<Action>::AsPtr(ir->extra("action"));
    Player* p = VPtr<Player>::AsPtr(ir->extra("chooser"));
    m_log << p->name() << "chose to" << *a;
    m_flushLog();
}

void LogicHumanPlayer::onArmorAdded(IORequest* ir)
{
    Hero* h = VPtr<Hero>::AsPtr(ir->extra("hero"));
    m_log << *h << "got its armor modified";
    m_flushLog();
    m_gui->refreshCard(h, h->owner() == m_player);
    ir->deleteLater();
}


void LogicHumanPlayer::onAttack(IORequest* ir)
{
    Character* atker = VPtr<Character>::AsPtr(ir->extra("attacker"));
    Character* target = VPtr<Character>::AsPtr(ir->extra("target"));
    m_log << *atker << "attacked" << *target;
    m_flushLog();
    m_gui->refreshCard(atker, atker->owner() == m_player);
    m_gui->refreshCard(target, target->owner() == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onBattlecry(IORequest* ir)
{
    Minion* issuingMinion = VPtr<Minion>::AsPtr(ir->extra("minion"));
    Action* bc = VPtr<Action>::AsPtr(ir->extra("action"));
    m_log << *issuingMinion << "executes its battlecry:" << *bc;
    m_flushLog();

    ir->deleteLater();
}

void LogicHumanPlayer::onCharacterDamagedOrHealed(IORequest* ir)
{
    Character* c = VPtr<Character>::AsPtr(ir->extra("target"));
    m_log << *c << "was"
          << (ir->type() == IORequest::DAMAGE_CHARACTER ? "damaged" : "healed");
    m_flushLog();

    m_gui->refreshCard(c, c->owner() == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onCardEnchanted(IORequest* ir)
{
    Card* c = VPtr<Card>::AsPtr(ir->extra("target"));
    m_log << *c << "enchantments changed";
    m_flushLog();

    m_gui->refreshCard(c, c->owner() == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onCardDrawn(IORequest* ir)
{
    if (ir->type() == IORequest::DRAW_FATIGUE) {
        Player* p = VPtr<Player>::AsPtr(ir->extra("player"));
        m_log << p->name() << "received a fatigue card!";
        m_flushLog();
    }
    else {
        Card* drawnCard = VPtr<Card>::AsPtr(ir->extra("card"));
        m_log << drawnCard->owner()->name() << "draws a card";
        m_flushLog();

        if (drawnCard->owner() == m_player) {
            m_gui->refreshDeck(m_player->deckSize(), true);
        }
        else {
            m_gui->refreshDeck(m_player->opponent()->deckSize(), false);
        }
    }
    ir->deleteLater();
}

void LogicHumanPlayer::onCardPutInHand(IORequest* ir)
{
    Card* cardPut = VPtr<Card>::AsPtr(ir->extra("card"));
    if (cardPut->owner() == m_player) {
        m_log << cardPut->owner()->name() << "gets a" << *cardPut->base()
                 << "put into his hand";
        m_flushLog();
        m_gui->addCardToHand(cardPut);
    }
    else {
        m_log << cardPut->owner()->name() << "gets a card put into his hand";
        m_flushLog();
        m_gui->addCardToEnemyHand();
    }

    ir->deleteLater();
}

void LogicHumanPlayer::onCardRemovedFromHand(IORequest* ir)
{
    Card* cardRemoved = VPtr<Card>::AsPtr(ir->extra("card"));
    m_log << cardRemoved->owner()->name() << "sees" << *cardRemoved->base()
          << "removed from his hand";
    m_flushLog();

    m_gui->removeCardFromHand(cardRemoved, cardRemoved->owner() == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onDeathRattle(IORequest* ir)
{
    Character* issuingChar = VPtr<Character>::AsPtr(ir->extra("character"));
    Action* dr = VPtr<Action>::AsPtr(ir->extra("action"));
    m_log << *issuingChar << "executes its death rattle:" << *dr;
    m_flushLog();

    ir->deleteLater();
}

void LogicHumanPlayer::onDrawnCardDestroyed(IORequest* ir)
{
    Card* drawnCard = VPtr<Card>::AsPtr(ir->extra("card"));
    m_log << drawnCard->owner()->name()
          << "has too many cards; the received" << *drawnCard->base()
          << "is put to graveyard";
    m_flushLog();
    ir->deleteLater();
}

void LogicHumanPlayer::onGameEnded(IORequest* ir)
{
    switch (ir->type()) {
        case IORequest::WON:
            m_log << m_player->name() << "won!";
            break;

        case IORequest::LOST:
            m_log << m_player->name() << "lost...";
            break;

        case IORequest::TIE:
            m_log << m_player->name() << "made a tie";
            break;

        default:
            qCritical() << "onGameEnded() received an unrecognized IORequest";
    }

    m_flushLog();
    ir->deleteLater();
}

void LogicHumanPlayer::onManaUpdate(IORequest* ir)
{
    Player* target = VPtr<Player>::AsPtr(ir->extra("target"));
    m_log << target->name() << "mana is updated:"
          << ir->extra("curMana").toInt() << "/"
          << ir->extra("maxMana").toInt();
    m_flushLog();
    m_gui->refreshMana(target, target == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onMinionDestroyed(IORequest* ir)
{
    Minion* destroyedMinion = VPtr<Minion>::AsPtr(ir->extra("minion"));
    m_log << *destroyedMinion << "was destroyed and put to graveyard";
    m_flushLog();

    m_gui->destroyMinion(destroyedMinion, destroyedMinion->owner() == m_player);
    ir->deleteLater();
}

void LogicHumanPlayer::onMinionSummoned(IORequest* ir)
{
    Minion* toSummon = VPtr<Minion>::AsPtr(ir->extra("minion"));
    int pos = ir->extra("position").toInt();
    m_log << *toSummon << "is summoned at position" << pos;
    m_flushLog();

    m_gui->summonMinion(toSummon, pos, toSummon->owner() == m_player);
}

void LogicHumanPlayer::onTargetChosen(IORequest* ir)
{
    Character* c = VPtr<Character>::AsPtr(ir->extra("target"));
    Player* p = VPtr<Player>::AsPtr(ir->extra("targetter"));
    m_log << p->name() << "targetted" << *c;
    m_flushLog();
}

void LogicHumanPlayer::onEventTriggered(IORequest* ir)
{
    Event* e = VPtr<Event>::AsPtr(ir->extra("event"));
    Action* a = VPtr<Action>::AsPtr(ir->extra("action"));
    m_log << *e << "-" << *a;
    m_flushLog();
}

void LogicHumanPlayer::askForAction(IORequest* ir)
{
    SelectActionTargetDialog* d = new SelectActionTargetDialog(ir, m_gui);
    d->exec();

    ir->setResponse((void*) VPtr<Action>::AsPtr(d->selection()));
    delete d;
}

void LogicHumanPlayer::askForPosition(IORequest* ir)
{
    int maxPos = VPtr<Minion>::AsPtr(ir->extra("minion"))->owner()->minions()->size();
    if (maxPos == 0) {
        ir->setResponse(new int(0));
        return;
    }

    SelectPositionDialog* d = new SelectPositionDialog(maxPos, m_gui);
    d->exec();

    int choice = d->selection();
    delete d;

    ir->setResponse(new int(choice));
}

void LogicHumanPlayer::askForTarget(IORequest* ir)
{
    SelectActionTargetDialog* d = new SelectActionTargetDialog(ir, m_gui);
    d->exec();

    ir->setResponse((void*) VPtr<Character>::AsPtr(d->selection()));
    delete d;
}
