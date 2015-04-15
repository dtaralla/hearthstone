#include "hero.h"
#include "heroidentity.h"
#include "actions/action.h"
#include "actions/specialpoweraction.h"
#include "player.h"
#include "iorequest.h"

Hero::Hero(const HeroIdentity* baseChar, Player* owner) :
    Character(baseChar, owner),
    m_armor(0),
    m_specialPower(NULL)
{
}

Hero::~Hero()
{
    if (m_specialPower != NULL)
        delete m_specialPower;
}

PlayAction* Hero::playAction()
{
    qCritical() << "A hero has no 'playAction'";
    return NULL;
}

QVector<Action*>* Hero::playActionConsequences() const
{
    qCritical() << "A hero has no 'playActionConsequences'";
    return NULL;
}

SpecialPowerAction* Hero::specialPower() const
{
    Q_CHECK_PTR(m_specialPower);
    return m_specialPower;
}

CardType Hero::type() const
{
    return CardTypes::CARD_HERO;
}

CharacterType Hero::subtype() const
{
    return CharacterTypes::HERO;
}

void Hero::initCard(Player* owner)
{
    Character::initCard(owner);
    Q_ASSERT(m_specialPower == NULL); // init should not be called more than once

    m_specialPower = (SpecialPowerAction*)((HeroIdentity*) m_base)->specialPower()->setSourceCard(this);
}

int Hero::armor() const
{
    return m_armor;
}

bool Hero::hasUsedSpecialPower() const
{
    return m_specialPower->alreadyUsed();
}

void Hero::setHasUsedSpecialPower(bool hasUsedSpecialPower)
{
    m_specialPower->setAlreadyUsed(hasUsedSpecialPower);
}

void Hero::addArmor(int quantity)
{
    int oldVal = m_armor;
    m_armor += quantity;
    if (m_armor < 0)
        m_armor = 0;

    if (oldVal != m_armor) {
        IORequest* ir = new IORequest(m_owner, IORequest::ADD_ARMOR);
        ir->putExtra<Hero>("hero", this);
        emit m_owner->armorAdded(ir);
    }
}

void Hero::modifyHp(int modification)
{
    if (modification < 0 && m_armor > 0) {
        m_armor = m_armor + modification;
        if (m_armor >= 0)
            return;

        modification = m_armor;
        m_armor = 0;
    }

    Character::modifyHp(modification);
}

