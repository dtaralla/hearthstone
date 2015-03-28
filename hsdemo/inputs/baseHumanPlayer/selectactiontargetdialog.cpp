#include "selectactiontargetdialog.h"
#include "stringable.h"
#include "iorequest.h"
#include "actions/action.h"
#include "character.h"
#include "guihumanplayer.h"

SelectActionTargetDialog::SelectActionTargetDialog(const IORequest* ir, QWidget *parent) :
    QDialog(parent, Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint),
    ui(new Ui::SelectActionTargetDialog),
    m_ir(ir)
{
    ui->setupUi(this);
    ui->buttonBox->setDisabled(true);

    if (ir->type() == IORequest::ACTION_SELECTION) {
        setWindowTitle("Select an action");
        QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));
        foreach (Action* a, *actions) {
            addSelection(VPtr<Action>::AsQVariant(a));
        }
    }
    else if (ir->type() == IORequest::TARGET_SELECTION) {
        setWindowTitle("Select a target");
        QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));
        foreach (Character* a, *targets)
            addSelection(VPtr<Character>::AsQVariant(a));
    }
    else {
        qCritical() << "This dialog is only meant to work with Action and Target selections!";
    }

    connect(&m_choices, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));

    m_choices.button(-2)->click();
}

SelectActionTargetDialog::~SelectActionTargetDialog()
{
    ((GUIHumanPlayer*)parent())->highlightAll(false);
    delete ui;
}


void SelectActionTargetDialog::addSelection(const QVariant& selection)
{
    Stringable* s = VPtr<Stringable>::AsPtr(selection);
    QRadioButton* b = new QRadioButton(*s);
    b->setProperty("value", selection);
    m_choices.addButton(b);

    ui->content->layout()->addWidget(b);
}

const QVariant& SelectActionTargetDialog::selection()
{
    return m_selection;
}

void SelectActionTargetDialog::accept()
{
    m_selection = m_choices.checkedButton()->property("value");
    hide();
}

void SelectActionTargetDialog::buttonClicked(int id)
{
    ui->buttonBox->setDisabled(false);

    Card* linkedCard = NULL;
    if (m_ir->type() == IORequest::ACTION_SELECTION) {
        linkedCard = VPtr<Action>::AsPtr(m_choices.button(id)->property("value"))->source();
    }
    else if (m_ir->type() == IORequest::TARGET_SELECTION) {
        linkedCard = VPtr<Character>::AsPtr(m_choices.button(id)->property("value"));
    }
    else {
        ((GUIHumanPlayer*)parent())->highlightAll(false);
    }

    if (linkedCard != NULL)
        ((GUIHumanPlayer*)parent())->highlight(linkedCard);
}


void SelectActionTargetDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);
    setModal(false);
}
