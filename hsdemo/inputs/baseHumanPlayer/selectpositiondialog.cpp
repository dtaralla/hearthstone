#include "selectpositiondialog.h"
#include "ui_selectpositiondialog.h"

SelectPositionDialog::SelectPositionDialog(int maxPosition, QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint |
            Qt::WindowTitleHint),
    ui(new Ui::SelectPositionDialog)
{
    ui->setupUi(this);
    ui->choice->setMaximum(maxPosition);
}

SelectPositionDialog::~SelectPositionDialog()
{
    delete ui;
}

int SelectPositionDialog::selection() const
{
    return ui->choice->value();
}

void SelectPositionDialog::accept()
{
    hide();
}


void SelectPositionDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);
    setModal(false);
}
