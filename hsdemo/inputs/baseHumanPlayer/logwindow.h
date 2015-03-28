#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QDialog>

namespace Ui {
class LogWindow;
}

class LogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = 0);
    ~LogWindow();

    void writeEntry(const QString& entry);

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
