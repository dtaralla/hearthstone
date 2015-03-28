#include <QVector>
#include <QtDebug>
#include <QtAlgorithms>
#include "player.h"
#include "playerinput.h"
#include "iorequest.h"
#include "minion.h"
#include "hero.h"
#include "vptr.h"
#include "actions/endturnaction.h"
#include "actions/playaction.h"
#include "spell.h"
#include "carddb.h"
#include "dbouput.h"
#include "game.h"

Player::Player(Hero* hero, PlayerInput* myInput, PlayerInput* hisInput,
               const QString& name) :
    QObject(),
    m_myInput(myInput),
    m_hisInput(hisInput),
    m_hero(hero),
    m_maxMana(0),
    m_curMana(0),
    m_endTurnAction(NULL),
    m_name(name),
    m_fatigue(-1)
{
    m_hero->initCard(this);
}

Player::~Player()
{
    delete m_endTurnAction;
    delete m_hero;
    delete m_myInput;
}

void Player::initPlayer()
{
    m_myInput->attachPlayer(this);
    m_endTurnAction = new EndTurnAction(game());

    // Connect the view to the Player's logic
    // Asynchronous signals
    connect(this, SIGNAL(actionChosen(IORequest*)),
            m_myInput, SLOT(onActionChosen(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(actionChosen(IORequest*)),
            m_hisInput, SLOT(onActionChosen(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(armorAdded(IORequest*)),
            m_myInput, SLOT(onArmorAdded(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(armorAdded(IORequest*)),
            m_hisInput, SLOT(onArmorAdded(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(attack(IORequest*)),
            m_myInput, SLOT(onAttack(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(attack(IORequest*)),
            m_hisInput, SLOT(onAttack(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(battlecry(IORequest*)),
            m_myInput, SLOT(onBattlecry(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(battlecry(IORequest*)),
            m_hisInput, SLOT(onBattlecry(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(characterDamagedOrHealed(IORequest*)),
            m_myInput, SLOT(onCharacterDamagedOrHealed(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(characterDamagedOrHealed(IORequest*)),
            m_hisInput, SLOT(onCharacterDamagedOrHealed(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(cardEnchanted(IORequest*)),
            m_myInput, SLOT(onCardEnchanted(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(cardEnchanted(IORequest*)),
            m_hisInput, SLOT(onCardEnchanted(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(cardDrawn(IORequest*)),
            m_myInput, SLOT(onCardDrawn(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(cardDrawn(IORequest*)),
            m_hisInput, SLOT(onCardDrawn(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(cardPutIntoHand(IORequest*)),
            m_myInput, SLOT(onCardPutInHand(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(cardPutIntoHand(IORequest*)),
            m_hisInput, SLOT(onCardPutInHand(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(cardRemovedFromHand(IORequest*)),
            m_myInput, SLOT(onCardRemovedFromHand(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(cardRemovedFromHand(IORequest*)),
            m_hisInput, SLOT(onCardRemovedFromHand(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(deathRattle(IORequest*)),
            m_myInput, SLOT(onDeathRattle(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(deathRattle(IORequest*)),
            m_hisInput, SLOT(onDeathRattle(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(drawnCardDestroyed(IORequest*)),
            m_myInput, SLOT(onDrawnCardDestroyed(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(drawnCardDestroyed(IORequest*)),
            m_hisInput, SLOT(onDrawnCardDestroyed(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(gameEnded(IORequest*)),
            m_myInput, SLOT(onGameEnded(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(manaUpdated(IORequest*)),
            m_myInput, SLOT(onManaUpdate(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(manaUpdated(IORequest*)),
            m_hisInput, SLOT(onManaUpdate(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(minionDestroyed(IORequest*)),
            m_myInput, SLOT(onMinionDestroyed(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(minionDestroyed(IORequest*)),
            m_hisInput, SLOT(onMinionDestroyed(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(minionSummoned(IORequest*)),
            m_myInput, SLOT(onMinionSummoned(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(minionSummoned(IORequest*)),
            m_hisInput, SLOT(onMinionSummoned(IORequest*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(targetChosen(IORequest*)),
            m_myInput, SLOT(onTargetChosen(IORequest*)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(targetChosen(IORequest*)),
            m_hisInput, SLOT(onTargetChosen(IORequest*)),
            Qt::QueuedConnection);


    // Synchronous signals
    connect(this, SIGNAL(eventTriggered(IORequest*)),
            m_myInput, SLOT(onEventTriggered(IORequest*)),
            Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(eventTriggered(IORequest*)),
            m_hisInput, SLOT(onEventTriggered(IORequest*)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(actionSelectionRequired(IORequest*)),
            m_myInput, SLOT(askForAction(IORequest*)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(positionSelectionRequired(IORequest*)),
            m_myInput, SLOT(askForPosition(IORequest*)),
            Qt::BlockingQueuedConnection);

    connect(this, SIGNAL(targetSelectionRequired(IORequest*)),
            m_myInput, SLOT(askForTarget(IORequest*)),
            Qt::BlockingQueuedConnection);
}

void Player::setDeck(const QVector<Card*>& deck, bool shuffleDeck)
{
    m_deck = deck;

    // Shuffle deck
    if (shuffleDeck)
        m_shuffleDeck();

    // Make sure all cards of the deck have their "owner" set to this player
    if (deck.first()->owner() == NULL) {
        foreach (Card* c, m_deck) {
            c->initCard(this);
        }
    }
}

void Player::newTurn()
{
    // Handle mana cristals creation/restore
    int maxMana;
    if (m_maxMana < 10)
        maxMana = m_maxMana + 1;
    else
        maxMana = m_maxMana;
    updateMana(maxMana, maxMana);

    // Make all minions able to attack again
    foreach (Minion* m, m_minions)
        m->setHasAttacked(false);

    // Make the Hero able to attack & use his special power again
    m_hero->setHasAttacked(false);
    m_hero->setHasUsedSpecialPower(false);

    // Draw card
    drawCard();
}

void Player::drawCard()
{
    if (m_deck.isEmpty()) {
        // Drawing from an empty deck makes you receive a fatigue card;
        // if it's the Nth fatigue drawn, it deals N dmg to your hero!
        IORequest* ir = new IORequest(this, IORequest::DRAW_FATIGUE);
        ir->putExtra<Player>("player", this);
        emit cardDrawn(ir);

        dealDamageHealing(NULL, m_hero, m_fatigue--);
    }
    else {
        Card* drawnCard = m_deck.takeFirst();

        // Tell the view a card has been drawn
        IORequest* ir = new IORequest(this, IORequest::DRAW_CARD);
        ir->putExtra<Card>("card", drawnCard);
        emit cardDrawn(ir);

        putCardInHand(drawnCard);
    }
}

void Player::updateMana(int maxMana, int curMana)
{
    if (maxMana < 0)
        maxMana = 0;
    if (curMana < 0)
        curMana = 0;

    // Put an Excess Mana spell for each mana cristal that could not be added to tray
    for (; maxMana > 10; maxMana -= 1) {
        Spell* excessMana = new Spell(CardDB::Instance()->cardInfo(CardDB::CARD__EXCESS_MANA));
        excessMana->initCard(this);
        putCardInHand(excessMana);
    }

    if (maxMana == m_maxMana && curMana == m_curMana)
        return;

    m_maxMana = maxMana;
    m_curMana = curMana;

    // Tell the view for mana update
    IORequest* ir_mana = new IORequest(this, IORequest::MANA_UPDATE);
    ir_mana->putExtra<Player>("target", this);
    ir_mana->putExtra("maxMana", m_maxMana);
    ir_mana->putExtra("curMana", m_curMana);
    emit manaUpdated(ir_mana);
}

void Player::modifyCurrentMana(int modification)
{
    updateMana(m_maxMana, m_curMana + modification);
}

void Player::modifyMaxMana(int modification)
{
    updateMana(m_maxMana + modification, m_curMana);
}

void Player::putCardInHand(Card* c)
{
    // Place the card in player's hand if possible
    if (m_hand.size() >= 10) {
        // Put drawn card directly in graveyard
        m_graveyard.append(c);

        // Tell the view the card was destroyed
        IORequest* ir = new IORequest(this, IORequest::DESTROY_CARD);
        ir->putExtra<Card>("card", c);
        emit drawnCardDestroyed(ir);
    }
    else {
        // Put card in player's hand
        m_hand.append(c);

        // Tell the view the card was put into hand
        IORequest* ir = new IORequest(this, IORequest::PUT_CARD_IN_HAND);
        ir->putExtra<Card>("card", c);
        emit cardPutIntoHand(ir);
    }
}

void Player::removeFromHand(Card* c)
{
    bool found = m_hand.removeOne(c);
    Q_ASSERT(found);

    // Tell the view the card was removed from hand
    IORequest* ir = new IORequest(this, IORequest::REMOVE_FROM_HAND);
    ir->putExtra<Card>("card", c);
    emit cardRemovedFromHand(ir);
}

void Player::win()
{
    IORequest* ir = new IORequest(this, IORequest::WON);
    emit gameEnded(ir);
}

void Player::loose()
{
    IORequest* ir = new IORequest(this, IORequest::LOST);
    emit gameEnded(ir);
}

void Player::tie()
{
    IORequest* ir = new IORequest(this, IORequest::TIE);
    emit gameEnded(ir);
}

void Player::summonMinion(Minion* m, int position)
{
    Q_ASSERT(m_minions.size() < 7);
    Q_ASSERT(m->owner() == this);
    if (position == -1 || position >= m_minions.size()) {
        m_minions.append(m);
        position = m_minions.size();
    }
    else
        m_minions.insert(position, m);

    // Tell the view
    IORequest* ir = new IORequest(this, IORequest::SUMMON_MINION);
    ir->putExtra<Minion>("minion", m);
    ir->putExtra("position", position);
    emit minionSummoned(ir);

    if (!m->hasAbility(Abilities::CHARGE))
        m->setHasAttacked(true); // Cannot attack at the turn it was summoned
}

void Player::putKilledMinionToGraveyard(Minion* m)
{
    // Logically put minion into graveyard
    bool found = m_minions.removeOne(m);
    Q_ASSERT(found);
    m_graveyard.append(m);

    // Tell the view the minion was destroyed/put in graveyard
    IORequest* ir = new IORequest(this, IORequest::DESTROY_MINION);
    ir->putExtra<Minion>("minion", m);
    emit minionDestroyed(ir);
}

void Player::executeAttack(Character* attacker, Character* target)
{
    Event e(Event::CHARACTER_ATTACKS);
    e.putExtra<Character>("attackingCharacter", attacker);
    e.putExtra<Character>("attackedCharacter", target);
    game()->trigger(e);

    // Tell the view an attack occured
    IORequest* ir = new IORequest(this, IORequest::ATTACK);
    ir->putExtra<Character>("attacker", attacker);
    ir->putExtra<Character>("target", target);
    emit attack(ir);

    if (attacker->dying() || target->dying())
        return;

    // Logically do the attack
    target->modifyHp(-attacker->atk());
    attacker->modifyHp(-target->atk());
    attacker->setHasAttacked(true);

    // TODO: this should be done in the order target/attacker were put in play?
    if (target->atk() > 0) {
        Event e(Event::CHARACTER_DAMAGED);
        e.putExtra<Card>("damager", target);
        e.putExtra<Character>("damagedCharacter", attacker);
        game()->trigger(e);
    }
    if (attacker->atk() > 0) {
        Event e(Event::CHARACTER_DAMAGED);
        e.putExtra<Card>("damager", attacker);
        e.putExtra<Character>("damagedCharacter", target);
        game()->trigger(e);
    }
}

void Player::dealDamageHealing(Card* dmgHealingSource, Character* target, int quantity)
{
    if (quantity == 0)
        return;

    // Do not trigger anything if healing does nothing
    if (quantity > 0 && target->hp() == target->maxHp())
        return;

    // Tell the view of what happened
    IORequest* ir = new IORequest(this, quantity > 0
                                        ? IORequest::HEAL_CHARACTER
                                        : IORequest::DAMAGE_CHARACTER);
    ir->putExtra<Character>("target", target);
    emit characterDamagedOrHealed(ir);

    // Logically cure/deal damage to target
    if (quantity > 0)
        target->heal(dmgHealingSource, quantity);
    else
        target->dealDmg(dmgHealingSource, -quantity);
}

Action* Player::selectAction(const QVector<Action*>* availableActions)
{
    Q_ASSERT(!availableActions->empty());

    IORequest* ir = new IORequest(this, IORequest::ACTION_SELECTION);
    ir->putExtra<QVector<Action*> >("availableActions", availableActions);
    emit actionSelectionRequired(ir);

    // e->response has been updated by slot now
    Q_CHECK_PTR(ir->response());

    Action* choice = (Action*) ir->response();
    delete ir;

    ir = new IORequest(this, IORequest::CHOOSE_ACTION);
    ir->putExtra<Action>("action", choice);
    ir->putExtra<Player>("chooser", this);
    emit actionChosen(ir);

    return choice;
}

int Player::selectPosition(const Minion* minionToPosition)
{
    IORequest* ir = new IORequest(this, IORequest::POSITION_SELECTION);
    ir->putExtra<Minion>("minion", minionToPosition);
    emit positionSelectionRequired(ir);

    // e->response has been updated by slot now
    Q_CHECK_PTR(ir->response());

    int choice = *((int*) ir->response());
    delete (int*) ir->response();
    delete ir;

    return choice;
}

Character* Player::selectTarget(const QVector<Character*>* availableTargets, Action* a)
{
    Q_ASSERT(!availableTargets->empty());

    IORequest* ir = new IORequest(this, IORequest::TARGET_SELECTION);

    ir->putExtra<QVector<Character*> >("availableTargets", availableTargets);
    ir->putExtra<Action>("action", a);
    emit targetSelectionRequired(ir);

    // e->response has been updated by slot now
    Q_CHECK_PTR(ir->response());

    Character* choice = (Character*) ir->response();
    delete ir;

    if (Game::IsDBGenerationModeOn())
        DBOutput::Instance(game())->buffer(choice, a);

    ir = new IORequest(this, IORequest::CHOOSE_TARGET);
    ir->putExtra<Character>("target", choice);
    ir->putExtra<Player>("targetter", this);
    emit targetChosen(ir);

    return choice;
}

Action* Player::endTurnAction()
{
    return m_endTurnAction;
}

Game* Player::game() const
{
    return (Game*) thread();
}

Player* Player::opponent() const
{
    return game()->opponentOf(this);
}

const QString& Player::name() const
{
    return m_name;
}

Hero* Player::hero()
{
    return m_hero;
}

int Player::currentMana() const
{
    return m_curMana;
}

int Player::maxMana() const
{
    return m_maxMana;
}

int Player::deckSize() const
{
    return m_deck.size();
}

QVector<Card*>* Player::playableCards()
{
    QVector<Card*>* toRet = new QVector<Card*>();
    foreach (Card* c, m_hand) {
        if (c->playAction()->resolvable())
            toRet->append(c);
    }

    return toRet;
}

const QVector<Card*>* Player::hand() const
{
    return &m_hand;
}

const QVector<Minion*>* Player::minions() const
{
    return &m_minions;
}

QVector<Character*>* Player::attackableCharacters() const
{
    // TODO: Stealth

    QVector<Character*>* targets = allTargets();

    QVector<Character*>* tauntingTargets = new QVector<Character*>();
    foreach (Character* c, *targets) {
        if (c->hasAbility(Abilities::TAUNT))
            *tauntingTargets << c;
    }

    if (tauntingTargets->size() > 0) {
        delete targets;
        return tauntingTargets;
    }
    else {
        delete tauntingTargets;
        return targets;
    }
}

QVector<Character*>* Player::allTargets() const
{
    QVector<Character*>* targets = new QVector<Character*>();
    foreach(Minion* m, m_minions)
        *targets << m;
    *targets << m_hero;
    return targets;
}

void Player::m_shuffleDeck()
{
    QVector<Card*>::iterator first = m_deck.begin();
    QVector<Card*>::iterator last  = m_deck.end();
    QVector<Card*>::difference_type i, n;
    n = last - first;
    for (i = 2; i < n; ++i)
        qSwap(first[i], first[qrand() % i]);
}

