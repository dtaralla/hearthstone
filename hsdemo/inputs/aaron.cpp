#include "aaron.h"
#include "player.h"
#include "game.h"
#include "character.h"
#include "actions/targetedaction.h"
#include "actions/attackaction.h"
#include "actions/playaction.h"
#include "iorequest.h"
#include <QCoreApplication>
#include <iostream>

Aaron::Aaron(QObject* parent) :
    ScriptedPlayer(parent),
    m_preselectedAttackTarget(NULL)
{
    if (!Py_IsInitialized()) {
        qDebug() << "Initializing Python Interpreter...";
        //Py_SetProgramName(QCoreApplication::arguments().at(0).toStdString());
        Py_Initialize();
        PySys_SetPath("../../hearthstone/scripts/");


        // Import init module
        PyObject* pyModuleName = PyString_FromString("init");
        Q_CHECK_PTR(pyModuleName);

        m_pyModule = PyImport_Import(pyModuleName);
        Q_CHECK_PTR(m_pyModule);
        Py_DecRef(pyModuleName);


        // Get callable functions
        m_pyPredPlayFunc = PyObject_GetAttrString(m_pyModule, "predPlay");
        m_pyPredTargetFunc = PyObject_GetAttrString(m_pyModule, "predTarget");
    }
}

Aaron::~Aaron()
{
    if (Py_IsInitialized()) {
        Py_DecRef(m_pyPredPlayFunc);
        Py_DecRef(m_pyPredTargetFunc);
        Py_DecRef(m_pyModule);
        Py_Finalize();
    }
}

void Aaron::askForAction(IORequest* ir)
{
    // actions only contains PlayActions, AttackActions and 1 EndTurnAction
    QVector<Action*>* actions = VPtr<QVector<Action*> >::AsPtr(ir->extra("availableActions"));
    if (actions->size() == 1) {
        std::cout << "Your turn!\n\n";
        ir->setResponse(actions->first());
        return;
    }

    const QVector<float> ENV = m_me->game()->environment();
    const int ENV_SIZE = ENV.size();

    Action* endTurn = NULL;
    QList<AttackAction*> atkActions;
    QList<PlayAction*> playActions;
    foreach (Action* a, *actions) {
        switch (a->type()) {
            case ActionTypes::ATTACK:
                atkActions.append((AttackAction*) a);
                break;

            case ActionTypes::PLAY_CARD:
                playActions.append((PlayAction*) a);
                break;

            case ActionTypes::END_TURN:
                endTurn = a;
                break;

            default:
                qCritical() << "AskForAction only gives Play, Atk or EndTurn";
                break;
        }
    }

    QList<AttackAction*> goodAttacks;
    QList<Character*> goodAttacks_target;

    if (!atkActions.empty()) {
        // Translate attack actions into predictable samples
        QVector<Character*>* targetsByAtk[atkActions.size()];
        PyObject* pyAtkToPredict  = PyList_New(0);
        int j = 0;
        foreach (AttackAction* a, atkActions) {
            // To each possible target corresponds a sample
            QVector<Character*>* atkables = m_me->opponent()->attackableCharacters();
            targetsByAtk[j++] = atkables;
            foreach (Character* c, *atkables) {
                PyObject* pyFeatures = PyList_New(ENV_SIZE + 5);

                // Put environment features in the list
                for (int i = 0; i < ENV_SIZE; i += 1) {
                    PyList_SET_ITEM(pyFeatures, i, PyFloat_FromDouble(ENV.at(i)));
                }

                // Put action-related features in the list
                PyList_SET_ITEM(pyFeatures, ENV_SIZE, PyLong_FromLong(a->id()));
                PyList_SET_ITEM(pyFeatures, ENV_SIZE + 1, PyLong_FromLong(c->base()->id()));
                PyList_SET_ITEM(pyFeatures, ENV_SIZE + 2, PyLong_FromLong(c->hp()));
                PyList_SET_ITEM(pyFeatures, ENV_SIZE + 3, PyLong_FromLong(c->atk()));
                PyList_SET_ITEM(pyFeatures, ENV_SIZE + 4, PyLong_FromLong(c->isSilenced()));

                // Put the sample in the list of targetted actions to predict
                PyList_Append(pyAtkToPredict, pyFeatures);
            }

        }

        // Get predictions in python format
        PyObject* pyArgs = PyTuple_New(1);
        PyTuple_SetItem(pyArgs, 0, pyAtkToPredict);
        PyObject* pyAtkProbas = PyObject_CallObject(m_pyPredTargetFunc, pyArgs);
        Py_DecRef(pyArgs);
        Py_DecRef(pyAtkToPredict);

        // Find acceptable attack actions
        int cumulativeTargetsPassed = 0;
        QMutableListIterator<AttackAction*> curActionIt(atkActions);
        curActionIt.next();
        Py_ssize_t size = PyList_Size(pyAtkProbas);
        const double THRESHOLD = 0.65;
        for (int i = 0; i < size; i += 1) {
            if (i >= cumulativeTargetsPassed + targetsByAtk[i]->size()) {
                Q_ASSERT(curActionIt.hasNext());
                curActionIt.next();
                cumulativeTargetsPassed = i;
            }

            double ithProba = PyFloat_AsDouble(PyList_GetItem(pyAtkProbas, i));
            if (ithProba >= THRESHOLD) {
                goodAttacks << curActionIt.value();
                goodAttacks_target << targetsByAtk[i]->at(i - cumulativeTargetsPassed);
            }
        }

        // Free memory
        Py_DecRef(pyAtkProbas);
        for (int i = 0; i < atkActions.size(); i+= 1) {
            delete targetsByAtk[i];
        }
    }

    QList<PlayAction*> goodPlays;
    if (!playActions.empty()) {
        // Translate play actions into predictable samples
        PyObject* pyPlayToPredict = PyList_New(playActions.size());
        foreach (PlayAction* a, playActions) {
            PyObject* pyFeatures = PyList_New(ENV_SIZE + 1);

            // Put environment features in the list
            for (int i = 0; i < ENV_SIZE; i += 1) {
                PyList_SET_ITEM(pyFeatures, i, PyFloat_FromDouble(ENV.at(i)));
            }

            // Put play-related features in the list
            PyList_SET_ITEM(pyFeatures, ENV_SIZE, PyLong_FromLong(a->source()->base()->id()));

            // Put the sample in the list of targetted actions to predict
            PyList_Append(pyPlayToPredict, pyFeatures);
        }

        // Get predictions in python format
        PyObject* pyArgs = PyTuple_New(1);
        PyTuple_SetItem(pyArgs, 0, pyPlayToPredict);
        PyObject* pyPlayProbas = PyObject_CallObject(m_pyPredPlayFunc, pyArgs);
        Py_DecRef(pyArgs);
        Py_DecRef(pyPlayToPredict);

        // Find acceptable play actions
        Py_ssize_t size = PyList_Size(pyPlayProbas);
        const double THRESHOLD = 0.68;
        for (int i = 0; i < size; i += 1) {
            double ithProba = PyFloat_AsDouble(PyList_GetItem(pyPlayProbas, i));
            if (ithProba >= THRESHOLD)
                goodPlays << playActions.at(i);
        }

        // Free memory
        Py_DecRef(pyPlayProbas);
    }


    // Select one action from the good ones (if none, end turn BUT chance to
    // take a random one ?!)
    if (goodAttacks.empty() && goodPlays.empty()) {
        std::cout << "Hmmm, I don't have any good actions...\n";
        if (qrand() % 3) {
            std::cout << "I prefer to do nothing and stop here. Your turn!\n\n";
            ir->setResponse(endTurn);
        }
        else {
            std::cout << "Let's take a scripted choice!\n";
            ScriptedPlayer::askForAction(ir);
        }

    }
    else {
        std::cout << "I determined that any action from the following list will help me gain board control:\n";
        const int size = goodAttacks.size();
        for (int i = 0; i < size; i += 1) {
            std::cout << '\t' << goodAttacks.at(i)->toString().toStdString() << ' '
                              << goodAttacks_target.at(i)->toString().toStdString() << '\n';
        }
        foreach (Action* a, goodPlays) {
            std::cout << '\t' << a->toString().toStdString() << '\n';
        }

        if (qrand() % 2) {
            int i = qrand() % size;
            Action* choice = goodAttacks.at(i);
            std::cout << "I choose to " << choice->toString().toStdString()
                      << ' ' << goodAttacks_target.at(i)->toString().toStdString() << '\n';
            ir->setResponse(choice);
        }
        else {
            Action* choice = goodPlays.at(qrand() % goodPlays.size());
            std::cout << "I choose to " << choice->toString().toStdString() << '\n';
            ir->setResponse(choice);
        }
    }
}

void Aaron::askForTarget(IORequest* ir)
{
    if (m_preselectedAttackTarget != NULL) {
        ir->setResponse(m_preselectedAttackTarget);

        // Consume the preselected target
        m_preselectedAttackTarget = NULL;
    }
    else {
        std::cout << "I need to choose a target.\n";

        QVector<Character*>* targets = VPtr<QVector<Character*> >::AsPtr(ir->extra("availableTargets"));
        Action* action = VPtr<Action>::AsPtr(ir->extra("action"));

        const QVector<float> ENV = m_me->game()->environment();
        const int ENV_SIZE = ENV.size();

        // Compile possible targetting situations for python
        PyObject* pyTargetToPredict  = PyList_New(targets->size());
        int i = 0;
        foreach (Character* c, *targets) {
            PyObject* pyFeatures = PyList_New(ENV_SIZE + 5);

            // Put environment features in the list
            for (int j = 0; j < ENV_SIZE; j += 1) {
                PyList_SET_ITEM(pyFeatures, j, PyFloat_FromDouble(ENV.at(i)));
            }

            // Put action-related features in the list
            PyList_SET_ITEM(pyFeatures, ENV_SIZE, PyLong_FromLong(((TargetedAction*) action)->id()));
            PyList_SET_ITEM(pyFeatures, ENV_SIZE + 1, PyLong_FromLong(c->base()->id()));
            PyList_SET_ITEM(pyFeatures, ENV_SIZE + 2, PyLong_FromLong(c->hp()));
            PyList_SET_ITEM(pyFeatures, ENV_SIZE + 3, PyLong_FromLong(c->atk()));
            PyList_SET_ITEM(pyFeatures, ENV_SIZE + 4, PyLong_FromLong(c->isSilenced()));

            // Put the sample in the list of targetted actions to predict
            PyList_SET_ITEM(pyTargetToPredict, i++, pyFeatures);
        }

        // Hand it to python to get predictions in python format
        PyObject* pyArgs = PyTuple_New(1);
        PyTuple_SetItem(pyArgs, 0, pyTargetToPredict);
        PyObject* pyTargetProbas = PyObject_CallObject(m_pyPredTargetFunc, pyArgs);
        Py_DecRef(pyArgs);
        Py_DecRef(pyTargetToPredict);

        // Find best target
        double bestProba = 0;
        Py_ssize_t size = PyList_Size(pyTargetProbas);
        for (int i = 0; i < size; i += 1) {
            double ithProba = PyFloat_AsDouble(PyList_GetItem(pyTargetProbas, i));
            if (ithProba >= bestProba) {
                ir->setResponse(targets->at(i));
                bestProba = ithProba;
            }
        }

        // Free memory
        Py_DecRef(pyTargetProbas);

        std::cout << "I select " << ((Character*) ir->response())->toString().toStdString()
                  << " and I am " << bestProba * 100 << "% confident in this choice.\n";
    }
}
