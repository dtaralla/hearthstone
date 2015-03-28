#ifndef SELECTPOSITIONDIALOG_H
#define SELECTPOSITIONDIALOG_H

#include <QDialog>

namespace Ui {
class SelectPositionDialog;
}

class SelectPositionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectPositionDialog(int maxPosition, QWidget *parent = 0);
    ~SelectPositionDialog();
    int selection() const;

private:
    Ui::SelectPositionDialog *ui;

    // QDialog interface
public slots:
    void accept();

    // QWidget interface
protected:
    void showEvent(QShowEvent* e);
};

#endif // SELECTPOSITIONDIALOG_H
