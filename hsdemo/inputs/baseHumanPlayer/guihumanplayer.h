#ifndef GUIHUMANPLAYER_H
#define GUIHUMANPLAYER_H

#include <QMainWindow>
#include <QHash>
#include <QTextStream>

namespace Ui {
class GUIHumanPlayer;
}

class Card;
class LogWindow;
class CardWidget;
class Player;
class Minion;
class SpecialPowerAction;
class Game;

class GUIHumanPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit GUIHumanPlayer(QWidget *parent = 0);
    ~GUIHumanPlayer();

    void setGame(Game* g);
    void initSpecialPower(SpecialPowerAction* sp, bool allyPlayer);
    void addCardToHand(Card* c);
    void addCardToEnemyHand();
    void removeCardFromHand(Card* c, bool allyPlayer);
    void destroyMinion(Minion* m, bool allyPlayer);

    void addEntryToLog(const QString& entry);
    LogWindow* logWindow();
    void highlightAll(bool hl);
    void highlight(Card* c);
    void refreshCard(Card* c, bool allyPlayer);
    void refreshMana(Player* p, bool allyPlayer);
    void refreshDeck(int deckSize, bool allyPlayer);
    void summonMinion(Minion* m, int position, bool allyPlayer);

public slots:
    void takeScreenshot(QTextStream& ts, QTextStream& ts2, QString fname);

private:
    Ui::GUIHumanPlayer* m_ui;
    LogWindow* m_log;
    QHash<Card*, CardWidget*> m_cards;
    Game* mGame;
};

#endif // GUIHUMANPLAYER_H
