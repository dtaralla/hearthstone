#include "randomplayer.h"
#include "actions/action.h"
#include "vptr.h"
#include "iorequest.h"
#include <QVector>
#include "character.h"
#include "player.h"
#include <QApplication>
#include <QFile>
#include <QLockFile>

RandomPlayer::RandomPlayer(bool writeResult) :
    PlayerInput(NULL),
    mMe(NULL),
    mWriteResults(writeResult)
{

}

RandomPlayer::~RandomPlayer()
{

}

void RandomPlayer::attachPlayer(Player* p)
{
    mMe = p;
}

void RandomPlayer::onGameEnded(IORequest *ir)
{
    if (!mWriteResults)
        return;

    static const QString fname = QString("scriptedrand_s%1.txt").arg(QApplication::arguments().at(9).toInt());

    QFile f(fname);
    QLockFile lock(f.fileName() + ".lock");

    lock.lock();
    bool opened = f.open(QFile::Append);
    Q_ASSERT(opened);
    switch (ir->type()) {
        case IORequest::LOST:
            f.write("0\n");
            break;

        case IORequest::WON:
            f.write("1\n");
            break;

        case IORequest::TIE:
            f.write("2\n");
            break;

        default:
            f.write("-1\n");
            break;
    }
    f.close();
    lock.unlock();

    ir->clearRef();
}



void RandomPlayer::onEventTriggered(IORequest* ir)
{
    Q_UNUSED(ir);
}

void RandomPlayer::askForAction(IORequest* ir)
{
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));

    // Let 3 turns advance to Nora
    /*if (mMe->maxMana() < 4) {
        foreach (Action* a, *actions) {
            if (a->type() == ActionTypes::END_TURN) {
                ir->setResponse(a);
                return;
            }
        }
    }*/

    // Subsequent turns? Play as long as possible
    int size = actions->size();
    if (size == 1) {
        ir->setResponse(actions->first());
        return;
    }

    Action* a = NULL;
    do {
        a = actions->at(qrand() % size);
    } while (a->type() == ActionTypes::END_TURN);

    ir->setResponse(a);
}

void RandomPlayer::askForPosition(IORequest* ir)
{
    ir->setResponse(new int(0));
}

void RandomPlayer::askForTarget(IORequest* ir)
{
    QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));
    ir->setResponse(targets->at(qrand() % targets->size()));
}
