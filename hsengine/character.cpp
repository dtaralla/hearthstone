#include "character.h"
#include "carddb.h"
#include "actions/attackaction.h"
#include "enchantment.h"
#include "triggers/trigger.h"
#include "player.h"
#include "game.h"

Character::Character(CharacterInfo const* baseChar, Player* owner) :
    Card(owner),
    m_base(baseChar),
    m_additionalAbilities(Abilities::NO_ABILITY),
    m_attackAction(new AttackAction(this)),
    m_maxHpModifier(0),
    m_curHp(baseChar->hp()),
    m_atkModifier(0),
    m_hasAttacked(false),
    m_dying(false),
    m_silenced(false),
    m_frozenEnchantment(NULL)
{
}

Character::~Character()
{
    delete m_attackAction;
}


const CardIdentity* Character::base() const
{
    return m_base;
}

CardType Character::type() const
{
    return CardTypes::CARD_CHARACTER;
}

CharacterType Character::characterType() const
{
    return CharacterTypes::CHARACTER;
}

Action* Character::attackAction() const
{
    return m_attackAction;
}

bool Character::hasAttacked() const
{
    return m_hasAttacked;
}

void Character::setHasAttacked(bool hasAttacked)
{
    m_hasAttacked = hasAttacked;
}

int Character::maxHp() const
{
    return m_base->hp() + m_maxHpModifier;
}

int Character::hp() const
{
    return m_curHp;
}

int Character::atk() const
{
    return m_base->atk() + m_atkModifier;
}

bool Character::dying() const
{
    return m_dying;
}

int Character::triggerPowersCount() const
{
    if (m_silenced)
        return m_additionalTriggerPowers.size();

    return m_triggerPowers.size() + m_additionalTriggerPowers.size();
}

void Character::dealDmg(Card* dmgSource, int dmg)
{
    Q_ASSERT(dmg >= 0);

    modifyHp(-dmg);
    if (dmg > 0) {
        Event e(Event::CHARACTER_DAMAGED);
        e.putExtra<Card>("damager", dmgSource);
        e.putExtra<Character>("damagedCharacter", this);
        owner()->game()->trigger(e);
    }
}

void Character::heal(Card* healingSource, int healing)
{
    Q_ASSERT(healing >= 0);

    modifyHp(healing);
    if (healing > 0) {
        Event e(Event::CHARACTER_HEALED);
        e.putExtra<Card>("healer", healingSource);
        e.putExtra<Character>("healedCharacter", this);
        owner()->game()->trigger(e);
    }
}

void Character::modifyHp(int modification)
{
    m_curHp += modification;

    int hpLimit = maxHp();
    if (m_curHp > hpLimit)
        m_curHp = hpLimit;
}

void Character::addMaxHPModifier(int modification)
{
    m_maxHpModifier += modification;

    // If bonus HP, curHp are also increased by the same value (NOT healing!)
    if (modification > 0)
        m_curHp += modification;

    // We can never have more HP than what the reserve offers
    int hpLimit = maxHp();
    if (m_curHp > hpLimit)
        m_curHp = hpLimit;
}

void Character::addAtkModifier(int modification)
{
    if (m_base->atk() + modification < 0)
        m_atkModifier = -m_base->atk();
    else
        m_atkModifier += modification;

}

void Character::addAbilities(Ability a)
{
    m_additionalAbilities |= a;
}

void Character::setDying(bool dying)
{
    m_dying = dying;
}

bool Character::injured() const
{
    return hp() < maxHp();
}

bool Character::hasAbility(Ability a) const
{
    if (m_silenced)
        return m_additionalAbilities & a;

    return m_base->hasAbility(a) || (m_additionalAbilities & a);
}

QVector<Enchantment*>* Character::takeEnchantmentsBefore(const Enchantment* s)
{
    QVector<Enchantment*>* taken = Card::takeEnchantmentsBefore(s);
    foreach (Enchantment* e, *taken) {
        if (e == m_frozenEnchantment) {
            m_frozenEnchantment = NULL;
        }

        if (e->maxHPModifier() > 0) {
            addMaxHPModifier(-e->maxHPModifier());
        }
        else
            m_maxHpModifier -= e->maxHPModifier();
    }

    return taken;
}

void Character::removeEnchantment(Enchantment* e)
{
    if (e->maxHPModifier() > 0) {
        addMaxHPModifier(-e->maxHPModifier());
    }
    else
        m_maxHpModifier -= e->maxHPModifier();

    Card::removeEnchantment(e);
}

void Character::clearEnchantmentEffects()
{
    Card::clearEnchantmentEffects();
    m_curHp -= m_maxHpModifier;
    m_maxHpModifier = 0;
    m_atkModifier = 0;
    m_additionalAbilities = Abilities::NO_ABILITY;
}

void Character::setSilenced(bool silenced)
{
    m_silenced = silenced;
}

bool Character::isSilenced() const
{
    return m_silenced;
}

bool Character::listensTo(const Event& e) const
{
    if (!m_silenced) {
        if (m_triggerPowers.contains(e.type())) {
            foreach (Trigger* t, *m_triggerPowers.value(e.type())) {
                if (t->listensTo(e))
                    return true;
            }
        }
    }

    if (m_additionalTriggerPowers.contains(e.type())) {
        foreach (Trigger* t, *m_additionalTriggerPowers.value(e.type())) {
            if (t->listensTo(e))
                return true;
        }
    }

    return false;
}

void Character::freeze(Card* sourceCard)
{
    if (m_frozenEnchantment != NULL)
        unfreeze();

    Enchantment* freeze =
            new Enchantment(false, "Frozen",
                            "This character looses its next attack.",
                            Abilities::FROZEN,
                            QHash<Event::Type, QVector<Trigger*>*>(),
                            0, 0, 0, 0);
    m_frozenEnchantment = freeze->specialize(sourceCard, this);
    delete freeze;

    addEnchantment(m_frozenEnchantment);
}

void Character::unfreeze()
{
    Q_CHECK_PTR(m_frozenEnchantment);
    removeEnchantment(m_frozenEnchantment);

    delete m_frozenEnchantment;
    m_frozenEnchantment = NULL;
}

