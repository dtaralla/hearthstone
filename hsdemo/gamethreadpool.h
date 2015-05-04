#ifndef GAMETHREADPOOL_H
#define GAMETHREADPOOL_H

#include <QVector>
#include <QThread>
#include <QSemaphore>
class GameController;

class GameThreadPool : public QThread
{
    Q_OBJECT

signals:
    void progress(int percent);

public:
    GameThreadPool(const QString& p1Hero, const QString& p2Hero, const QString& p1Deck, const QString& p2Deck, int howMany,
                   uint maxThreadCount = 1000, QObject* parent = NULL);
    ~GameThreadPool();

    void run();

public slots:
    void threadFinished();

private:
    QString mP1Hero;
    QString mP2Hero;
    QString mP1Deck;
    QString mP2Deck;
    int mHowMany;
    QSemaphore mSem;
    QSemaphore mExitCondition;
    QVector<GameController*> mPool;
};

#endif // GAMETHREADPOOL_H
