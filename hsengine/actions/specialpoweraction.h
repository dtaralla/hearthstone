#ifndef SPECIALPOWERACTION_H
#define SPECIALPOWERACTION_H

#include "action.h"

class SpecialPowerAction : public Action
{
public:
    SpecialPowerAction(int id, const QString& name, const QString& desc, int manaCost,
                       QVector<Action*>* effects);
    ~SpecialPowerAction();

    ActionType type() const;
    void resolve(const Event* e = NULL);
    Action* cloneFlyweight() const;
    bool resolvable(const Event* e = NULL) const;
    Action* setSourceCard(Card* c) const;
    const QString& name() const;
    const QString& desc() const;
    int cost() const;
    void setAlreadyUsed(bool used);
    bool alreadyUsed() const;
    const QVector<Action*>* effects() const;
    int id() const;

private:
    const int m_id;
    const QString m_name;
    const QString m_desc;
    const int m_cost;
    QVector<Action*>* m_effects;
    bool m_alreadyUsed;


    // Stringable interface
public:
    QString toString() const;
};

#endif // SPECIALPOWERACTION_H
