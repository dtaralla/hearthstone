#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>

namespace Ui {
class CardWidget;
}

class Card;

class CardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CardWidget(Card* c, QWidget *parent = NULL);
    ~CardWidget();

    void refresh();
    void highlight(bool hl);
    QString tooltipString() const;

private:
    Ui::CardWidget* ui;
    Card* m_card;
};


#endif // CARDWIDGET_H

