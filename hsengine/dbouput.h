#ifndef DBOUTPUT_H
#define DBOUTPUT_H

#include <QList>
#include <QTextStream>
#include <QFile>

class Action;
class Character;
class Game;

class DBOutput
{
public:
    static DBOutput* Instance(Game* g);
    static void DestroyInstance(Game* g);

    void buffer(const QVector<float>& environment, Action* a);
    void buffer(Character* target, Action* a);
    void buffer(float score, Action *a);
    void addEntry(const QVector<float>& environment, Action* a, float score);

private:
    enum BufferEntryType {
        ENVIRONMENT,
        TARGET
    };

    static QHash<Game*, DBOutput*> mInstances;
    QString mPlayActionFile;
    QString mTargetedActionFile;
    QTextStream mOs_playAction;
    QTextStream mOs_targetedAction;
    QStringList mBuffer;
    BufferEntryType mLastInsertionInBuffer;

    DBOutput();
    ~DBOutput();
    void mInsertEnvironment(const QVector<float>& environment, QTextStream &os);
    void mInsertTarget(const Character* target, QTextStream &os);
};

#endif // DBOUTPUT_H
