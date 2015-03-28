#include "guihumanplayer.h"
#include "ui_guihumanplayer.h"
#include "cardwidget.h"
#include "logwindow.h"

#include "game.h"
#include "card.h"
#include "hero.h"
#include "player.h"
#include "minion.h"
#include "enchantment.h"
#include "actions/specialpoweraction.h"
#include <QPixmap>
#include <QScreen>
#include <QToolTip>

GUIHumanPlayer::GUIHumanPlayer(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::GUIHumanPlayer),
    mGame(NULL)
{
    m_ui->setupUi(this);
    showMaximized();

    m_log = new LogWindow();
    m_log->show();
}

GUIHumanPlayer::~GUIHumanPlayer()
{
    m_log->hide();
    delete m_log;
    delete m_ui;
}

void GUIHumanPlayer::setGame(Game* g)
{
    mGame = g;
}

void GUIHumanPlayer::initSpecialPower(SpecialPowerAction* sp, bool allyPlayer)
{
    if (allyPlayer) {
       m_ui->spName->setText(sp->name());
       m_ui->spCost->setText(QString("Cost: %1").arg(sp->cost()));
       m_ui->spDesc->appendPlainText(sp->desc());
       m_ui->hero->setTitle(sp->source()->owner()->hero()->name());
       refreshCard(sp->source(), true);
       refreshMana(sp->source()->owner(), true);
    }
    else {
        m_ui->eSpName->setText(sp->name());
        m_ui->eSpCost->setText(QString("Cost: %1").arg(sp->cost()));
        m_ui->eSpDesc->appendPlainText(sp->desc());
        m_ui->eHero->setTitle(sp->source()->owner()->hero()->name());
        refreshCard(sp->source(), false);
        refreshMana(sp->source()->owner(), false);
    }
}

void GUIHumanPlayer::addCardToHand(Card* c)
{
    CardWidget* cw = new CardWidget(c);
    m_cards.insert(c, cw);
    ((QHBoxLayout*)m_ui->hand->layout())->addWidget(cw);
}

void GUIHumanPlayer::addCardToEnemyHand()
{
    QLabel* c = new QLabel();
    c->setPixmap(QPixmap(QString::fromUtf8(":/img/cardBack.png")));
    c->setMinimumSize(150, 224);
    c->setMaximumSize(c->minimumSize());

    ((QHBoxLayout*)m_ui->eHand->layout())->addWidget(c);
}

void GUIHumanPlayer::removeCardFromHand(Card* c, bool allyPlayer)
{
    if (allyPlayer) {
        CardWidget* cw = m_cards.value(c, NULL);
        Q_CHECK_PTR(cw);
        m_ui->hand->layout()->removeWidget(cw);
        cw->hide(); // Else glitch; stay in the hand until reparented
        if (c->type() & CardTypes::CARD_SPELL) {
            delete cw;
            m_cards.remove(c);
        }
    }
    else {
        QLayoutItem* cardBack = m_ui->eHand->layout()->takeAt(0);
        Q_CHECK_PTR(cardBack);
        delete cardBack->widget();
        delete cardBack;
    }
}

void GUIHumanPlayer::destroyMinion(Minion* m, bool allyPlayer)
{
    CardWidget* cw = m_cards.take(m);
    Q_CHECK_PTR(cw);

    if (allyPlayer) {
        m_ui->hand->layout()->removeWidget(cw);
    }
    else {
        m_ui->eHand->layout()->removeWidget(cw);
    }

    delete cw;
}

void GUIHumanPlayer::addEntryToLog(const QString& entry)
{
    m_log->writeEntry(entry);
}

LogWindow* GUIHumanPlayer::logWindow()
{
    return m_log;
}

void GUIHumanPlayer::highlightAll(bool hl)
{
    foreach (CardWidget* cw, m_cards.values())
        cw->highlight(hl);
}

void GUIHumanPlayer::highlight(Card* c)
{
    highlightAll(false);
    if (c->type() == CardTypes::CARD_HERO)
        return;

    Q_ASSERT(m_cards.contains(c));
    m_cards.value(c)->highlight(true);
}

void GUIHumanPlayer::refreshCard(Card* c, bool allyPlayer)
{
    switch (c->type()) {
        case CardTypes::CARD_MINION:
        case CardTypes::CARD_SPELL: {
            Q_ASSERT(m_cards.contains(c));
            m_cards.value(c)->refresh();
            break;
        }

        case CardTypes::CARD_HERO: {
            Hero* h = (Hero*) c;
            QString enchantments("Enchantments:");
            if (h->enchantments()->empty())
                enchantments += " None";
            else {
                enchantments += "<ol>";
                foreach (Enchantment* e, *h->enchantments()) {
                    enchantments += "<li><b>" + e->name() + ":</b> " + e->toString() + "</li>";
                }
                enchantments += "</ol>";
            }

            if (allyPlayer) {
                m_ui->heroHp->setText(QString("<b>HP:</b> %1").arg(h->hp()));
                m_ui->heroAtk->setText(QString("<b>ATK:</b> %1").arg(h->atk()));
                m_ui->heroArmor->setText(QString("<b>Armor:</b> %1").arg(h->armor()));
                m_ui->hero->setToolTip(enchantments);
            }
            else {
                m_ui->eHeroHp->setText(QString("<b>HP:</b> %1").arg(h->hp()));
                m_ui->eHeroAtk->setText(QString("<b>ATK:</b> %1").arg(h->atk()));
                m_ui->eHeroArmor->setText(QString("<b>Armor:</b> %1").arg(h->armor()));
                m_ui->eHero->setToolTip(enchantments);
            }
            break;
        }

        default:
            qCritical() << "Cannot refresh other cards than Spells, minions and heroes";
    }
}

void GUIHumanPlayer::refreshMana(Player* p, bool allyPlayer)
{
    QLabel* toEdit = allyPlayer ? m_ui->mana : m_ui->eMana;
    toEdit->setText(QString("<b>Mana:</b> %1/%2").arg(p->currentMana()).arg(p->maxMana()));
}

void GUIHumanPlayer::refreshDeck(int deckSize, bool allyPlayer)
{
    if (allyPlayer) {
        if (deckSize == 0)
            m_ui->deckQ->setStyleSheet("color: black");
        m_ui->deckQ->setText(QString("%1").arg(deckSize));
    }
    else {
        if (deckSize == 0)
            m_ui->eDeckQ->setStyleSheet("color: black");
        m_ui->eDeckQ->setText(QString("%1").arg(deckSize));
    }
}

void GUIHumanPlayer::summonMinion(Minion* m, int position, bool allyPlayer)
{
    if (allyPlayer) {
        if (m_cards.contains(m)) {
            // This is a PlayAction consequence
            CardWidget* cw = m_cards.value(m);
            ((QHBoxLayout*)m_ui->minions->layout())->insertWidget(position, cw);
            cw->show(); // It was hidden in removeCardFromHand because of the glitch
        }
        else {
            // This is a SummonNewMinionsAction consequence
            CardWidget* cw = new CardWidget(m);
            m_cards.insert(m, cw);
            ((QHBoxLayout*)m_ui->minions->layout())->insertWidget(position, cw);
        }
    }
    else {
        CardWidget* cw = new CardWidget(m);
        m_cards.insert(m, cw);
        ((QHBoxLayout*)m_ui->eMinions->layout())->insertWidget(position, cw);
    }
}

void GUIHumanPlayer::takeScreenshot(QTextStream& ts, QTextStream& ts2, QString fname)
{
    if (mGame == NULL)
        return;

    ts << "Picture " << fname << '\n';
    ts << "====================================================\n";
    ts << "Board control score:\n";
    Game::BoardControlScore* s = mGame->meta_BoardControlScore();
    ts << s->TOUGH_MINIONS_MULT << "*" << s->toughMinionsScore << "+\n"
       << s->HAND_MULT << "*" << s->handScore << "+\n"
       << s->MINIONS_MULT << "*" << s->minionsScore << "+\n"
       << s->SITUATION_MULT << "*" << s->situationScore << "\n=>"
       << s->score;

    ts << "\nEnchantments:\n";
    foreach (Card* c, m_cards.keys()) {
        ts << c->toString() << '\n' << m_cards.value(c)->tooltipString();
    }
    ts << "\n\n";

    ts2 << s->score << "\n";
    delete s;

    repaint();
    QGuiApplication::primaryScreen()->grabWindow(winId()).save(fname);
}
