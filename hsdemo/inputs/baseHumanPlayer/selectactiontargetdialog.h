#ifndef SELECTACTIONTARGETDIALOG_H
#define SELECTACTIONTARGETDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include "ui_selectactiontargetdialog.h"
#include "vptr.h"
class IORequest;

class SelectActionTargetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectActionTargetDialog(const IORequest* ir, QWidget *parent = 0);
    ~SelectActionTargetDialog();

    void addSelection(const QVariant& selection);
    const QVariant& selection();

private:
    Ui::SelectActionTargetDialog *ui;
    QVariant m_selection;
    QButtonGroup m_choices;
    const IORequest* m_ir;


    // QDialog interface
public slots:
    void accept();
    void buttonClicked(int id);

    // QWidget interface
protected:
    void showEvent(QShowEvent* e);
};


#endif // SELECTACTIONTARGETDIALOG_H
