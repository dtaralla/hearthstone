#ifndef AARON_H
#define AARON_H

#include "scriptedplayer.h"
#include <Python.h>

class Player;
class Character;

class Aaron : public ScriptedPlayer
{
    Q_OBJECT

public:
    Aaron(QObject* parent = NULL);
    virtual ~Aaron();

private:
    Character* m_preselectedAttackTarget;
    PyObject* m_pyModule;
    PyObject* m_pyPredBoardCtrlPlayFunc;
    PyObject* m_pyPredBoardCtrlTargetFunc;
    PyObject* m_pyPredAggroPlayFunc;
    PyObject* m_pyPredAggroTargetFunc;

    void mSelectBestBCAction(IORequest* ir);
    void mSelectBestAction(IORequest* ir);

    // ScriptedPlayer inherited members
public slots:
    void askForAction(IORequest* ir);
    void askForTarget(IORequest* ir);
};

#endif // AARON_H
