#ifndef CONSOLEPROGRESSBAR_H
#define CONSOLEPROGRESSBAR_H

#include <QtCore/QObject>

class ConsoleProgressBar : public QObject
{
    Q_OBJECT
public:
    static ConsoleProgressBar* Instance();
    ~ConsoleProgressBar();

signals:

public slots:
    void update(int progress);

private:
    static ConsoleProgressBar* mInstance;
    QString mProgressBars;

    explicit ConsoleProgressBar(QObject *parent = 0);
};

#endif // CONSOLEPROGRESSBAR_H
