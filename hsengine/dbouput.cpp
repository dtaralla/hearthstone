#include "dbouput.h"
#include "actions/action.h"
#include "actions/attackaction.h"
#include "actions/targetedaction.h"
#include "actions/specialpoweraction.h"
#include "character.h"
#include "game.h"

#include <QLockFile>
#include <QDir>

QHash<int, QHash<int, DBOutput*>*> DBOutput::mInstances;
QString DBOutput::mFilenamesPrefix = "db";


DBOutput::DBOutput(ScoreType st) :
    mPlayActionFile(""),
    mTargetedActionFile(""),
    mAttackActionFile(""),
    mOs_playAction(&mPlayActionFile),
    mOs_targetedAction(&mTargetedActionFile),
    mOs_attackAction(&mAttackActionFile)

{
    QDir().mkdir("generated");
    switch (st) {
        case AGGRO:
            mScoreTypePrefix = "aggro";
            break;

        case BOARD_CONTROL:
            mScoreTypePrefix = "boardCtrl";
            break;
    }
}

DBOutput::~DBOutput()
{
    QStringList ls;

    ls << "generated/" + mScoreTypePrefix + "." + mFilenamesPrefix + ".play.csv"
       << "generated/" + mScoreTypePrefix + "." + mFilenamesPrefix + ".target.csv"
       << "generated/" + mScoreTypePrefix + "." + mFilenamesPrefix + ".atk.csv";

    QStringList contents;
    contents << mPlayActionFile << mTargetedActionFile << mAttackActionFile;

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

DBOutput* DBOutput::Instance(Game* g, ScoreType st)
{
    QHash<int, DBOutput*>* map = mInstances.value(g->id(), NULL);
    if (map == NULL) {
        map = new QHash<int, DBOutput*>();
        map->reserve(2);
        mInstances.insert(g->id(), map);
    }

    DBOutput* instance = map->value(st, NULL);
    if (instance == NULL) {
        instance = new DBOutput(st);
        map->insert(st, instance);
    }

    return instance;
}

void DBOutput::DestroyInstance(Game* g)
{
    QHash<int, DBOutput*>* map = mInstances.take(g->id());
    if (map != NULL) {
        foreach (DBOutput* dbo, *map)
            delete dbo;
        delete map;
    }
}

void DBOutput::DestroyInstance(Game* g, ScoreType st)
{
    QHash<int, DBOutput*>* map = mInstances.value(g->id());
    if (map != NULL) {
        delete map->take(st);

        if (map->empty()) {
            mInstances.remove(g->id());
            delete map;
        }
    }
}

void DBOutput::SetFilenamesPrefix(const QString& prefix)
{
    mFilenamesPrefix = prefix;
}

void DBOutput::buffer(const QVector<float>& environment, Action* a)
{
    if (!a->isTargetedAction())
        qCritical() << "This function should only be used along with targeted actions:" << a->toString();

    mBuffer.append(QString(""));
    QTextStream os(&mBuffer.last());
    mInsertEnvironment(environment, os);
    if (a->type() == ActionTypes::ATTACK) {
        Character* source = (Character*) a->source();
        os << source->base()->id() << " " << source->hp() << " " << source->atk() << " " << source->isSilenced() << " ";
    }
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

    int owner = 0;
    if (target->owner() == a->source()->owner())
        owner = 1;
    os << target->base()->id() << " " << owner << " " << target->hp() << " " << target->atk() << " " << target->isSilenced() << " ";

    mLastInsertionInBuffer = TARGET;
}

void DBOutput::buffer(float score, Action* a)
{
    if (!a->isTargetedAction())
        qCritical() << "This function should only be used along with targeted actions:" << a->toString();

    if (mLastInsertionInBuffer != TARGET)
        qCritical() << "Buffer environment, then target, then score:" << a->toString();

    QTextStream* os;
    if (a->type() == ActionTypes::ATTACK)
        os = &mOs_attackAction;
    else
        os = &mOs_targetedAction;
    *os << mBuffer.takeLast() << score << '\n';
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


