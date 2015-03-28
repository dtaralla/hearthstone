#include "aaron.h"
#include "player.h"

Aaron::Aaron(QObject* parent) :
    PlayerInput(parent),
    m_me(NULL),
    m_enemy(NULL)
{

}

Aaron::~Aaron()
{

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
}

void Aaron::askForPosition(IORequest* ir)
{
}

void Aaron::askForTarget(IORequest* ir)
{
}
