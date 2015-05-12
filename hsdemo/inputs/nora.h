#ifndef NORA_H
#define NORA_H

#include "scriptedplayer.h"
#include <Python.h>

class Player;
class Character;

class Nora : public ScriptedPlayer
{
    Q_OBJECT

public:
    Nora(bool writeResult = false, QObject* parent = NULL);
    virtual ~Nora();

private:
    bool mWriteResult;
    Character* m_preselectedAttackTarget;
    static uint m_nbNoras;
    static PyObject* m_pyModule;
    static PyObject* m_pyPredBoardCtrlPlayFunc;
    static PyObject* m_pyPredBoardCtrlTargetFunc;
    static PyObject* m_pyPredBoardCtrlAtkFunc;
    static PyObject* m_pyPredAggroPlayFunc;
    static PyObject* m_pyPredAggroTargetFunc;

    void mSelectBestBCAction(IORequest* ir);
    void mSelectBestAction(IORequest* ir);

    // ScriptedPlayer inherited members
public slots:
    void onGameEnded(IORequest* ir);
    void askForAction(IORequest* ir);
    void askForTarget(IORequest* ir);
};

#endif // NORA_H
