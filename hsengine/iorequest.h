#ifndef INPUTREQUEST_H
#define INPUTREQUEST_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QString>
#include "vptr.h"

class Player;

class IORequest : public QObject
{
    Q_OBJECT

public:
    enum RequestType {
        ADD_ARMOR,
        ATTACK,
        BATTLECRY,
        CAST_SPELL,
        CHOOSE_ACTION,
        CHOOSE_TARGET,
        DAMAGE_CHARACTER,
        DEATHRATTLE,
        DESTROY_CARD,
        DESTROY_MINION,
        DRAW_CARD,
        DRAW_FATIGUE,
        ENCHANT_CHARACTER,
        HEAL_CHARACTER,
        LOST,
        MANA_UPDATE,
        PUT_CARD_IN_HAND,
        REMOVE_FROM_HAND,
        SUMMON_MINION,
        TIE,
        TRIGGER,
        WON,

        ACTION_SELECTION,
        POSITION_SELECTION,
        TARGET_SELECTION
    };

    explicit IORequest(Player* source, RequestType type, int refCount=2);
    ~IORequest();

    RequestType type() const;
    QVariant extra(const QString& key) const;
    void putExtra(const QString& key, const QVariant& value);
    template<class T> void putExtra(const QString& key, const T* value)
    {
        putExtra(key, VPtr<T>::AsQVariant(value));
    }

    void setResponse(void* r);
    void* response() const;
    void clearRef();

private:
    int mRefCount;
    Player* m_source;
    RequestType m_type;
    QHash<QString, QVariant> m_extras;

    void* m_response;
};

#endif // INPUTREQUEST_H
