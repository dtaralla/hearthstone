#include "aaron.h"
#include "player.h"
#include <Python.h>
#include <QCoreApplication>

#ifdef QT_NO_DEBUG
#define PATH "../../../hearthstone/db/"
#else
#define PATH "../../hearthstone/db/"
#endif

Aaron::Aaron(QObject* parent) :
    PlayerInput(parent),
    m_me(NULL),
    m_enemy(NULL),
    m_preselectedAttackTarget(NULL)
{
    if (!Py_IsInitialized()) {
        qDebug() << "Initializing Python Interpreter...";
        Py_SetProgramName(QCoreApplication::arguments().at(0).toStdString().data());
        Py_Initialize();

        // Run init script
        const char* INIT_SCRIPT_NAME = "../../hearthstone/scripts/init.py";
        PyRun_SimpleFileEx(fopen(INIT_SCRIPT_NAME, "r"), INIT_SCRIPT_NAME, 0);
    }
}

Aaron::~Aaron()
{
    if (Py_IsInitialized())
        Py_Finalize();
}

void Aaron::attachPlayer(Player* p)
{
    m_me = p;
    m_enemy = p->opponent();
}


void Aaron::onEventTriggered(IORequest* ir)
{
}

void Aaron::askForAction(IORequest* ir)
{
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));

    // actions only contains PlayActions, AttackActions and 1 EndTurnAction
    foreach (Action* a, *actions) {
        PyObject* pyFeatures =
    }

    ir->setResponse(actions->at(qrand() % actions->size()));
}

void Aaron::askForPosition(IORequest* ir)
{
}

void Aaron::askForTarget(IORequest* ir)
{
}
