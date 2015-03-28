#ifndef TRIGGER_H
#define TRIGGER_H

#include <QVector>
#include "game_globals.h"

class Action;
class Event;
class Card;

class Trigger
{
public:
    Trigger(QVector<Action*>* actions);
    Trigger(Action* action);
    virtual ~Trigger();

    virtual QVector<Action*>* actions() const;
    virtual bool listensTo(const Event& e) const = 0;
    virtual Trigger* clone() const = 0;
    virtual Trigger* setSourceCard(Card* c) const;


protected:
    Card* m_card;
    QVector<Action*>* m_actions;
};

#endif // TRIGGER_H
