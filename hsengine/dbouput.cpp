#include "dbouput.h"
#include "actions/action.h"
#include "actions/attackaction.h"
#include "actions/targetedaction.h"
#include "actions/specialpoweraction.h"
#include "character.h"
#include "game.h"

#include <QLockFile>
#include <QDir>

QHash<int, DBOutput*> DBOutput::mInstances;


DBOutput::DBOutput() :
    mPlayActionFile(""),
    mTargetedActionFile(""),
    mOs_playAction(&mPlayActionFile),
    mOs_targetedAction(&mTargetedActionFile)

{
    QDir().mkdir("generated");
}

DBOutput::~DBOutput()
{
    QStringList ls;

    ls << "generated/db.play.csv"
       << "generated/db.target.csv";

    QStringList contents;
    contents << mPlayActionFile << mTargetedActionFile;

    const int size = contents.size();
    for (int i = 0; i < size; i += 1) {
        QFile f(ls.at(i));
        QLockFile lock(f.fileName() + ".lock");

        lock.lock();
        bool opened = f.open(QFile::Append);
        Q_ASSERT(opened);
        f.write(contents.at(i).toLatin1());
        f.close();
        lock.unlock();
    }
}

DBOutput* DBOutput::Instance(Game* g)
{
    DBOutput* instance = mInstances.value(g->id(), NULL);
    if (instance == NULL) {
        instance = new DBOutput();
        mInstances.insert(g->id(), instance);
    }

    return instance;
}

void DBOutput::DestroyInstance(Game* g)
{
    delete mInstances.take(g->id());
}

void DBOutput::buffer(const QVector<float>& environment, Action* a)
{
    if (!a->isTargetedAction())
        qCritical() << "This function should only be used along with targeted actions:" << a->toString();

    mBuffer.append(QString(""));
    QTextStream os(&mBuffer.last());
    mInsertEnvironment(environment, os);
    if (a->type() == ActionTypes::ATTACK)
        os << ((AttackAction*) a)->id() << " ";
    else
        os << ((TargetedAction*) a)->id() << " ";

    mLastInsertionInBuffer = ENVIRONMENT;
}

void DBOutput::buffer(Character *target, Action *a)
{
    if (!a->isTargetedAction())
        qCritical() << "This function should only be used along with targeted actions:" << a->toString();

    if (mLastInsertionInBuffer != ENVIRONMENT)
        qCritical() << "Buffer environment, then target, then score:" << a->toString();

    QTextStream os(&mBuffer.last());
    mInsertTarget(target, os);
    mLastInsertionInBuffer = TARGET;
}

void DBOutput::buffer(float score, Action* a)
{
    if (!a->isTargetedAction())
        qCritical() << "This function should only be used along with targeted actions:" << a->toString();

    if (mLastInsertionInBuffer != TARGET)
        qCritical() << "Buffer environment, then target, then score:" << a->toString();

    mOs_targetedAction << mBuffer.takeLast() << score << '\n';
}

void DBOutput::addEntry(const QVector<float>& environment, Action* a, float score)
{
    if (a->isTargetedAction())
        qCritical() << "Use the buffer(...) functions, designed for targeted actions";

    else if (a->type() == ActionTypes::PLAY_CARD) {
        mInsertEnvironment(environment, mOs_playAction);
        mOs_playAction << a->source()->base()->id() << " " << score << '\n';
    }
    else if (a->type() == ActionTypes::SPECIAL_POWER) {
        mInsertEnvironment(environment, mOs_playAction);
        mOs_playAction << ((SpecialPowerAction*) a)->id() << " " << score << '\n';
    }
    else
        qCritical() << "Trying to add entry for action other than playcard/spPower:" << a->toString();
}

void DBOutput::mInsertEnvironment(const QVector<float>& environment, QTextStream& os)
{
    foreach (float f, environment)
        os << f << " ";
}

void DBOutput::mInsertTarget(const Character* target, QTextStream& os)
{
    os << target->base()->id() << " " << target->hp() << " " << target->atk() << " " << target->isSilenced() << " ";
}

