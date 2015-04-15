#include "cardwidget.h"
#include "ui_cardwidget.h"
#include "card.h"
#include "minion.h"
#include "spell.h"
#include "enchantment.h"

CardWidget::CardWidget(Card* c, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardWidget),
    m_card(c)
{
    ui->setupUi(this);
    if (m_card->type() == CardTypes::CARD_SPELL)
        ui->back->setPixmap(QPixmap(QString::fromUtf8(":/img/emptySpell.png")));
    ui->back->setToolTipDuration(30000);

    if (c->type() == CardTypes::CARD_MINION)
        ui->type->setText(CharacterTypes::toString(((Minion*)c)->subtype()));

    ui->hasAttacked->setVisible(false);
    ui->isFrozen->setVisible(false);

    refresh();
}

CardWidget::~CardWidget()
{
    delete ui;
}

void CardWidget::refresh()
{
    ui->manaCost->setText(QString("%1").arg(m_card->effectiveManaCost()));
    ui->name->setText(m_card->name());
    ui->desc->setText(m_card->base()->desc());

    if (m_card->type() == CardTypes::CARD_MINION) {
        Minion* m = (Minion*) m_card;
        ui->atk->setText(QString("%1").arg(m->atk()));
        ui->hp->setText(QString("%1").arg(m->hp()));
        if (m->isInjured())
            ui->hp->setStyleSheet("color: yellow");
        else
            ui->hp->setStyleSheet("color: white");

        ui->hasAttacked->setVisible(m->hasAttacked());
        ui->isFrozen->setVisible(m->hasAbility(Abilities::FROZEN));
    }

    QString enchantments("Enchantments:");
    if (m_card->enchantments()->empty())
        enchantments += " None\n";
    else {
        enchantments += "<ol>\n";
        foreach (Enchantment* e, *m_card->enchantments()) {
            enchantments += "<li><b>" + e->name() + ":</b> " + e->toString() + "</li>\n";
        }
        enchantments += "</ol>";
    }
    ui->back->setToolTip(enchantments);
}

void CardWidget::highlight(bool hl)
{
    if (hl)
        ui->back->setStyleSheet("border: 1px solid blue");
    else
        ui->back->setStyleSheet("");
}

QString CardWidget::tooltipString() const
{
    return ui->back->toolTip();
}
