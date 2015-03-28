#include "logwindow.h"
#include "ui_logwindow.h"

LogWindow::LogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint |
                   Qt::WindowMinimizeButtonHint |
                   Qt::Dialog);
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::writeEntry(const QString& entry)
{
    ui->log->appendPlainText(entry);
    ui->log->ensureCursorVisible();
}
