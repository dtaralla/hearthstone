#include "game.h"
#include "carddb.h"
#include "player.h"
#include "spell.h"
#include "character.h"
#include "hero.h"
#include "minion.h"
#include "actions/attackaction.h"
#include "actions/specialpoweraction.h"
#include "event.h"
#include "iorequest.h"
#include "dbouput.h"
#include "playerinput.h"
#include <QCoreApplication>

#define SHUFFLE_HAND_MODE 0



namespace TradeUtils {

int bestTrade(Player* atker, Player* target, bool ignoreHasAttacked)
{
    int bestTrade = -99999999;
    int bestScore = -99999999;

    QVector<Minion*> curAllies;
    foreach (Minion* m, *atker->minions()) {
        if ((ignoreHasAttacked || !m->hasAttacked()) && !m->hasAbility(Abilities::FROZEN))
            curAllies << m;
    }
    QVector<Minion*> curEnemies(*target->minions());

    if (curEnemies.size() == 0) {
        if (atker->minions()->size() == 0)
            return 0;

        if (curAllies.size() > 0)
            return 0.5;

        //then (atker->minions()->size() > 0)
        return 0.25;

    }
    else if (atker->minions()->size() == 0)
        return -0.5;


    QMap<int, HP_ATK*>* allies = new QMap<int, HP_ATK*>();
    minionsToStructs(&curAllies, allies);

    QMap<int, HP_ATK*>* otherEnemies, *attackableEnemies;
    tauntablesToStructs(&curEnemies, &attackableEnemies, &otherEnemies);

    int maxIndex = attackableEnemies->size();
    int curTradeScore = 0;
    int i = 1; // Used to loop on all attackable enemies
               // (try trades beginning with maxIndex different atkable enemies)
    int j = 1;
    mainLoop: while (j <= attackableEnemies->size()) {
        if (j > attackableEnemies->size() || j <= 0)
            qCritical() << "This is not supposed to happen... with j =" << j;

        // Pick the jth strongest enemy
        HP_ATK* strongestEnemy = attackableEnemies->values().at(attackableEnemies->size() - j);

        while (!allies->empty()) {
            // Pick the weakest ally; if equality give priority to the one having best atk
            HP_ATK* weakestAlly = allies->values().first();
            const QList<HP_ATK*> weakestAllies = allies->values(weakestAlly->hp + weakestAlly->atk);
            if (weakestAllies.size() > 1) {
                // Weakest ally is the one with best ATK
                foreach (HP_ATK* ha, weakestAllies) {
                    if (ha->atk > weakestAlly->atk)
                        weakestAlly = ha;
                }
            }

            // Perform attack
            int enemKey = strongestEnemy->hp + strongestEnemy->atk;
            int allyKey = weakestAlly->hp + weakestAlly->atk;
            strongestEnemy->hp -= weakestAlly->atk;
            weakestAlly->hp -= strongestEnemy->atk;

            // Compute attack score
            if (strongestEnemy->hp <= 0)
                curTradeScore += enemKey;
            else {
                curTradeScore += weakestAlly->atk;
                if (weakestAlly->atk > 0)
                    updateItemPosition(attackableEnemies, enemKey, strongestEnemy);
            }

            if (weakestAlly->hp <= 0) {
                curTradeScore -= allyKey;
            }
            else {
                curTradeScore -= strongestEnemy->atk;
            }
            removeItem(allies, allyKey, weakestAlly);

            // If there are still allies living and the enemy died, continue
            // the trade score computation (if there are still enemies...)
            if (strongestEnemy->hp <= 0 && !allies->empty()) {
                removeItem(attackableEnemies, enemKey, strongestEnemy);

                if (j > 1 && j > attackableEnemies->size())
                    j -= 1; // go backward towards the strongest enemies, beginning with the weakests

                if (attackableEnemies->empty() && otherEnemies != NULL) {
                    delete attackableEnemies;
                    attackableEnemies = otherEnemies;
                    otherEnemies = NULL;
                    j = 1; // Reset current target counter
                }

                if (!attackableEnemies->empty()) {
                    continue_label(mainLoop);
                }
                else
                    break; // There remains allies but no more enemies!
            }
        }

        // Update best trade if needed
        if (curTradeScore > bestTrade) {
            bestTrade = curTradeScore;
            bestScore = 0;
            foreach (HP_ATK* ha, *attackableEnemies)
                bestScore -= ha->atk + ha->hp;
            if (otherEnemies != NULL)
                foreach (HP_ATK* ha, *otherEnemies)
                    bestScore -= ha->atk + ha->hp;

            foreach (HP_ATK* ha, *allies)
                bestScore += ha->atk + ha->hp;
        }

        // Try another trade, starting on (i+1)th strongest now
        qDeleteAll(*attackableEnemies);
        delete attackableEnemies;
        attackableEnemies = NULL;
        if (otherEnemies != NULL) {
            qDeleteAll(*otherEnemies);
            delete otherEnemies;
            otherEnemies = NULL;
        }
        qDeleteAll(*allies);
        allies->clear();

        i += 1;
        j = i;

        // Do not refresh list if there are no more trades to try
        if (i <= maxIndex) {
            minionsToStructs(&curAllies, allies);
            tauntablesToStructs(&curEnemies, &attackableEnemies, &otherEnemies);
            curTradeScore = 0;
        }
        else {
            delete allies;
            allies = NULL;
            break;
        }
    }

    if (allies != NULL) {
        qDeleteAll(*allies);
        delete allies;
    }
    if (attackableEnemies != NULL) {
        qDeleteAll(*attackableEnemies);
        delete attackableEnemies;
    }
    if (otherEnemies != NULL) {
        qDeleteAll(*otherEnemies);
        delete otherEnemies;
    }

    return bestScore / 20.;
}

void removeItem(QMap<int, HP_ATK*>* map,  int key, HP_ATK* item) {
    // Note: the item HAS to be in the map!!!

    QMap<int,HP_ATK*>::iterator i = map->find(key);
    while (true) {
        if (i.value() == item) {
            map->erase(i);
            delete item;
            break;
        }
        ++i;
    }
}

void updateItemPosition(QMap<int, HP_ATK*>* map, int key, HP_ATK* item) {
    // Note: the item HAS to be in the map!!!

    QMap<int, HP_ATK*>::iterator i = map->find(key);
    while (true) {
        if (i.value() == item) {
            map->erase(i);
            break;
        }
        ++i;
    }

    map->insertMulti(item->hp + item->atk, item);
}

void tauntablesToStructs(const QVector<Minion*>* minions, QMap<int, HP_ATK*>** atkables, QMap<int, HP_ATK*>** others) {
    *others = NULL;
    *atkables = new QMap<int, HP_ATK*>();
    foreach (Minion* m, *minions) {
        HP_ATK* info = new HP_ATK();
        info->atk = m->atk();
        info->hp = m->hp();

        if (m->hasAbility(Abilities::TAUNT)) {
            if (others == NULL) {
                *others = *atkables;
                *atkables = new QMap<int, HP_ATK*>();
            }

            (*atkables)->insertMulti(info->atk + info->hp, info);
        }
        else if (*others == NULL)
            (*atkables)->insertMulti(info->atk + info->hp, info);
        else
            (*others)->insertMulti(info->atk + info->hp, info);
    }

    // In case there are only taunts
    if (*others != NULL && (*others)->empty()) {
        delete *others;
        *others = NULL;
    }
}

void minionsToStructs(const QVector<Minion*>* minions, QMap<int, HP_ATK*>* structs) {
    foreach (Minion* m, *minions) {
        HP_ATK* info = new HP_ATK();
        info->atk = m->atk();
        info->hp = m->hp();

        structs->insertMulti(info->atk + info->hp, info);
    }
}

} // TradeUtils namespace




bool Game::m_dbGenerationMode = false;
int Game::mNextGameID = 0;

QVector<Action*>* Game::mAttackActions()
{
    QVector<Action*>* atkActions = new QVector<Action*>();

    // Take all minions that have not attacked yet on this turn
    foreach (Minion* m, *(m_curPlayer->minions())) {
        if (m->hasAttacked() || m->atk() <= 0 || m->hasAbility(Abilities::FROZEN))
            continue;

        *atkActions << m->attackAction();
    }

    // Can the hero attack?
    Hero* h = m_curPlayer->hero();
    if (h->atk() > 0 && !h->hasAttacked())
        *atkActions << h->attackAction();

    return atkActions;
}


bool Game::IsDBGenerationModeOn()
{
    return m_dbGenerationMode;
}

void Game::InitializeGlobals(bool DBGenerationModeOn)
{
    m_dbGenerationMode = DBGenerationModeOn;
}

Game::Game(const QString& p1Name, Hero* p1Hero, const QString& p1Deck, PlayerInput* p1Input,
           const QString& p2Name, Hero* p2Hero, const QString& p2Deck, PlayerInput* p2Input) :
    QObject(NULL),
    mId(mNextGameID++),
    m_player1(new Player(p1Hero, p1Input, p2Input, p1Name)),
    m_player2(new Player(p2Hero, p2Input, p1Input, p2Name))
{
    // Player inputs live in the main thread which has the default event loop.
    // This way, they are able to receive signals emitted by the Player objects.
    // On the contrary, as Players live in this thread, which has NO event loop
    // if start() is called instead of exec(), these won't be able to receive
    // signals even though they are able to send some.

    // Attach decks to players
    if (p1Deck.isEmpty())
        CardDB::Instance()->buildRandomDeck(m_player1);
    else
        CardDB::Instance()->buildDeckFromFile(p1Deck, m_player1, true);

    if (p2Deck.isEmpty())
        CardDB::Instance()->buildRandomDeck(m_player2);
    else
        CardDB::Instance()->buildDeckFromFile(p2Deck, m_player2, true);

    // Get first and second players
    if (qrand() % 2) {
        m_curPlayer = m_player1;
        m_nextPlayer = m_player2;
    }
    else {
        m_curPlayer = m_player2;
        m_nextPlayer = m_player1;
    }
}

Game::~Game()
{
    delete m_player1;
    delete m_player2;
    DBOutput::DestroyInstance(this);

}

void Game::initPlayers(QThread* moveTo)
{
    if (moveTo != NULL) {
        // Put players in this thread, but not their inputs
        m_player1->moveToThread(moveTo);
        m_player2->moveToThread(moveTo);
    }

    // Init them
    m_player1->initPlayer();
    m_player2->initPlayer();
}

void Game::quitGame()
{
    emit finished();
}

void Game::enterGameLoop()
{
    // Give starting hand to 1st player
    m_curPlayer->drawCard();
    m_curPlayer->drawCard();
    m_curPlayer->drawCard();

    // Give starting hand to second player
    m_nextPlayer->drawCard();
    m_nextPlayer->drawCard();
    m_nextPlayer->drawCard();
    m_nextPlayer->drawCard();

    // TODO: Mulligan phase comes here

    // Create "The coin" and give it to second player
    Spell* theCoin = new Spell(CardDB::Instance()->cardInfo(CardDB::CARD__THE_COIN));
    theCoin->initCard(m_nextPlayer);
    m_nextPlayer->putCardInHand(theCoin);


    // Make the first player draw a card and have an additional mana cristal
    m_curPlayer->newTurn();

    // Game loop
    while (!m_player1->hero()->dying() && !m_player2->hero()->dying()) {
        // Get available actions for him
        QVector<Action*>* availableActions = new QVector<Action*>();

        // -> Always possible to end turn
        availableActions->append(m_curPlayer->endTurnAction());

        // -> Get playable cards actions
        QVector<Card*>* availableCards = m_curPlayer->playableCards();
        foreach (Card* c, *availableCards)
            availableActions->append((Action*) c->playAction());
        delete availableCards;

        // -> Get attack actions
        QVector<Action*>* attackActions = mAttackActions();
        foreach (Action* a, *attackActions)
            availableActions->append(a);

        // Can the hero use the special power?
        if (m_curPlayer->hero()->specialPower()->resolvable())
            availableActions->append((Action*) m_curPlayer->hero()->specialPower());


        // Make the player choose one
        Action* choice = m_curPlayer->selectAction(availableActions);


        // Resolve the action
        mUpdateState(choice);


        // Clean temporary actions & containers
        delete attackActions;
        delete availableActions;
    }


    // Determine winner and looser
    bool tie = m_player1->hero()->dying() && m_player2->hero()->dying();
    if (tie) {
        m_player1->tie();
        m_player2->tie();
    }
    else if (m_player1->hero()->dying()) {
        m_player1->loose();
        m_player2->win();
    }
    else {
        m_player1->win();
        m_player2->loose();
    }

    quitGame();
}

void Game::mUpdateState(Action* move, const Event* e)
{
    Q_CHECK_PTR(move);

    // TODO: Clear and re-apply temporary/conditional effects

    if (IsDBGenerationModeOn() && move->isTargetedAction())
        DBOutput::Instance(this)->buffer(environment(), move);

    // Resolve action
    bool moveDeleted = move->deletesItselfAfterResolve();
    move->resolve(e);

    // Act the deaths which occured during this phase (they can't be healed now)
    if (m_player1->hero()->hp() <= 0) {
        m_player1->hero()->setDying(true);
    }
    if (m_player2->hero()->hp() <= 0) {
        m_player2->hero()->setDying(true);
    }

    bool deathsActingAlreadyBegun = !m_deathsQueue.empty();
    for (int i = 0; i < m_boardCardsByPlayTime.size(); i += 1) {
        Card* c = m_boardCardsByPlayTime.at(i);
        if (!(c->type() & CardTypes::CARD_MINION))
            continue;

        Minion* m = (Minion*) c;
        if (m->hp() <= 0) {
            m_deathsQueue.append(m);
            m_boardCardsByPlayTime.remove(i--);
            m->owner()->putKilledMinionToGraveyard(m);
            m->setDying(true);
        }
    }

    // Only then, run any deathrattles (if not already done higer in the stack!)
    if (!deathsActingAlreadyBegun) {
        while (!m_deathsQueue.empty()) {
            Minion* m = m_deathsQueue.first();
            QVector<Action*>* deathRattles = m->deathRattles();
            foreach (Action* a, *deathRattles) {
                IORequest* ir = new IORequest(m->owner(), IORequest::DEATHRATTLE);
                ir->putExtra<Character>("character", m);
                ir->putExtra<Action>("action", a);
                emit m->owner()->deathRattle(ir);

                mUpdateState(a);
            }
            delete deathRattles;

            Event ev(Event::MINION_DIES);
            ev.putExtra<Minion>("dyingMinion", m);
            trigger(ev); // TODO: ATTENTION - Secrets should NOT be triggered
                         // during this catchup phase! (they are the only ones)
                         // Maybe this should be put before updateState loop - Ref:
                         // https://twitter.com/bdbrode/status/564293434660237312
            m_deathsQueue.removeFirst();
        }
    }

    if (moveDeleted || move->source() == NULL)
        return;

    // For moves issued from cards, trigger "AFTER_XXX" effects
    if (move->source()->type() & (CardTypes::CARD_MINION | CardTypes::CARD_SPELL)) {
        Event e(Event::AFTER_CARD_PLAYED);
        e.putExtra<Card>("card", move->source());
        trigger(e);
    }

    if (IsDBGenerationModeOn() && move->isTargetedAction()) {
        BoardControlScore* score = meta_BoardControlScore();
        DBOutput::Instance(this)->buffer(score->score, move);
        delete score;
    }
}

void Game::nextTurn()
{
    // First unfreeze any frozen char. that has already lost its attack
    QVector<Character*>* cs = m_curPlayer->allTargets();
    foreach (Character* c, *cs) {
        if (c->hasAbility(Abilities::FROZEN) && !c->hasAttacked())
            c->unfreeze();
    }
    delete cs;

    // Trigger end of turn effects
    Event e(Event::AT_TURN_END);
    e.putExtra<Player>("player", m_curPlayer);
    trigger(e);

    // NEW TURN!
    Player* tmp = m_curPlayer;
    m_curPlayer = m_nextPlayer;
    m_nextPlayer = tmp;

    // Make the player draw a card and have an additional mana cristal
    m_curPlayer->newTurn();
}

void Game::summonMinion(Minion* toPlay, int position)
{
    toPlay->owner()->summonMinion(toPlay, position);
    m_boardCardsByPlayTime.append(toPlay);
}

QVector<float> Game::environment() const
{
    QVector<float> toRet;

    /***************************************************************
     * If 3 minions, these are the possible positions:             *
     * 1 2 3 / / / /                                               *
     * / 1 2 3 / / /                                               *
     * / / 1 2 3 / /                                               *
     * / / / 1 2 3 /                                               *
     * / / / / 1 2 3                                               *
     *                                                             *
     * Choose one randomly amongst them -> this way, classificator *
     * does not always find the first cards the most important!    *
     **************************************************************/
    const QVector<Minion*>* minions = m_curPlayer->minions();
    int minionCount = minions->size();
    int insertAt = (qrand() % (7 - minionCount + 1));
    int endAt = insertAt + minionCount;
    for (int i = 0; i < 7; i += 1) {
        // Put placeholders for missing minions: -1 0 -9999 -9999 0 0
        if (i < insertAt || i >= endAt)
            toRet << -1 << 0 << -9999 << -9999 << 0 << 0;
        // Put ally minions info: ID CAN_ATTACK HP ATK SILENCED ENCHANTED
        else {
            Minion* m = minions->at(i - insertAt);
            toRet << m->base()->id();
            if (m->hasAttacked() || m->hasAbility(Abilities::FROZEN))
                toRet << 0;
            else
                toRet << 1;
            toRet << m->hp() << m->atk() << m->isSilenced() << !m->enchantments()->empty();
        }
    }

    /* Do the same for enemy minions */
    minions = m_nextPlayer->minions();
    minionCount = minions->size();
    insertAt = (qrand() % (7 - minionCount + 1));
    endAt = insertAt + minionCount;
    for (int i = 0; i < 7; i += 1) {
        // Put placeholders for missing minions: -1 0 -9999 -9999 0 0
        if (i < insertAt || i >= endAt)
            toRet << -1 << 0 << -9999 << -9999 << 0 << 0;
        // Put enemy minions info: ID CAN_ATTACK HP ATK SILENCED ENCHANTED
        else {
            Minion* m = minions->at(i - insertAt);
            toRet << m->base()->id();
            if (m->hasAttacked() || m->hasAbility(Abilities::FROZEN))
                toRet << 0;
            else
                toRet << 1;
            toRet << m->hp() << m->atk() << m->isSilenced() << !m->enchantments()->empty();
        }
    }


    if (SHUFFLE_HAND_MODE)
        mAddShuffledHandTo(toRet);
    else
        mAddHandAsVectorTo(toRet);


    // Append various simple data
    int enemMaxMana = m_nextPlayer->maxMana() + 1;
    if (enemMaxMana > 10)
        enemMaxMana = 10;

    toRet << m_curPlayer->minions()->size()
          << m_nextPlayer->minions()->size()
          << m_curPlayer->hand()->size()
          << m_nextPlayer->hand()->size()
          << m_curPlayer->currentMana()
          << m_curPlayer->maxMana()
          << enemMaxMana;
          //<< m_curPlayer->deckSize()
          //<< m_nextPlayer->deckSize();

    return toRet;
}

void Game::mAddShuffledHandTo(QVector<float>& environment) const
{
    /***************************************************************
     * If 2 cards, these are the possible positions:               *
     * 1 2 / / / / / / / /                                         *
     * 1 / 2 / / / / / / /                                         *
     * 1 / / 2 / / / / / /                                         *
     * 1 / / / 2 / / / / /                                         *
     * ...                                                         *
     * 2 1 / / / / / / / /                                         *
     * 2 / 1 / / / / / / /                                         *
     * ...                                                         *
     * / 1 2 / / / / / / /                                         *
     * / 1 / 2 / / / / / /                                         *
     * ...                                                         *
     * / 2 1 / / / / / / /                                         *
     * / 2 / 1 / / / / / /                                         *
     * ...                                                         *
     *                                                             *
     * In short: all permutations of 1 2 / / / / / / / /           *
     * Choose one randomly amongst them -> this way, classificator *
     * does not always find the first cards the most important!    *
     **************************************************************/
    QVector<Card*> cards(*m_curPlayer->hand());

    if (cards.empty())
        environment << QVector<float>(10, -1);
    else {
        QVector<float> tmpList;
        tmpList.reserve(10);
        int tmpListCount = cards.size();

        // Put ally hand info: ID, in random order
        while (!cards.empty())
            tmpList << cards.takeAt(qrand() % cards.size())->base()->id();

        // Put placeholders for missing cards: -1, at random locations
        while (tmpListCount < 10) {
            tmpList.insert(qrand() % tmpListCount++, -1);
        }

        // Append result to environment
        environment << tmpList;
    }
}

void Game::mAddHandAsVectorTo(QVector<float>& environment) const
{
    const int maxId = CardDB::Instance()->maxCardId();
    const int offset = environment.size();
    environment << QVector<float>(maxId + 1, 0);

    foreach (Card* c, *m_curPlayer->hand())
        environment[offset + c->base()->id()] += 1;
}

void Game::trigger(const Event& e)
{

    // TODO: secrets, hand cards (?)

    // Get listening cards
    QVector<Card*> listeningCards;
    if (m_player1->hero()->listensTo(e))
        listeningCards << m_player1->hero();

    if (m_player2->hero()->listensTo(e))
        listeningCards << m_player2->hero();

    foreach (Card* c, m_boardCardsByPlayTime) {
        if (c->listensTo(e))
            listeningCards.append(c);
    }

    // Execute each actions AFTER (no more activations once the phase has begun)
    foreach (Card* c, listeningCards) {
        const QVector<Action*>* toTrigger = c->actionsOnEvent(e);
        foreach (Action* a, *toTrigger) {
            // Tell the view something triggered
            IORequest* ir = new IORequest(c->owner(), IORequest::TRIGGER);
            ir->putExtra<Event>("event", &e);
            ir->putExtra<Action>("action", a);
            emit c->owner()->eventTriggered(ir);
            delete ir;

            // Execute action
            mUpdateState(a, &e);
        }
        delete toTrigger;
    }
}

int Game::id() const
{
    return mId;
}

QVector<Character*>* Game::attackableTargets(const Character* atker) const
{
    return opponentOf(atker->owner())->attackableCharacters();
}

Player* Game::opponentOf(const Player* p) const
{
    Q_CHECK_PTR(m_player1);
    Q_CHECK_PTR(m_player2);
    return (p == m_player1) ? m_player2 : m_player1;
}

QVector<Card*>* Game::playedCardsByPlayTime(Player* p, Owner o) const
{
    QVector<Card*>* toRet = new QVector<Card*>(m_boardCardsByPlayTime);

    // Remove minions still in the course of their battlecry
    for (int i = 0; i < toRet->size(); i += 1) {
        if (toRet->at(i)->type() == CardTypes::CARD_MINION && !((Minion*) toRet->at(i))->battlecryResolved())
            toRet->remove(i--);
    }

    if (o == Owners::ANY_OWNER)
        return toRet;

    Q_ASSERT(p == m_player1 || p == m_player2);

    if (o & Owners::ALLY) {
        for (int i = 0; i < toRet->size(); i += 1) {
            if (toRet->at(i)->owner() != p)
                toRet->remove(i--);
        }
    }
    else {
        for (int i = 0; i < toRet->size(); i += 1) {
            if (toRet->at(i)->owner() == p)
                toRet->remove(i--);
        }
    }

    return toRet;
}

QVector<Character*>* Game::charactersByPlayTime(Player* p, Owner o, CharacterType ct) const
{
    QVector<Card*>* cards = playedCardsByPlayTime(p, o);
    QVector<Character*>* chars = new QVector<Character*>();
    if (ct & CharacterTypes::HERO) {
        if (o & Owners::ALLY)
            chars->append(p->hero());
        if (o & Owners::ENEMY)
            chars->append(opponentOf(p)->hero());
    }

    foreach (Card* c, *cards) {
        if (c->type() == CardTypes::CARD_MINION && ((Character*) c)->characterType() & ct)
            chars->append((Character*) c);
    }
    delete cards;

    return chars;
}


Game::BoardControlScore* Game::meta_BoardControlScore()
{
    Player* curPl = m_curPlayer;
    Player* oppPl = opponentOf(m_curPlayer);
    BoardControlScore* score = new BoardControlScore;

    /* Tough minions score
     *      = 0    if #allyTough == #enemyTough
     *      = 1/3  if #allyTough == #enemyTough + 1
     *      = 2/3  if #allyTough == #enemyTough + 2
     *      = 3/3  if #allyTough >= #enemyTough + 3
     *      = -1/3 if #allyTough == #enemyTough - 1
     *      = -2/3 if #allyTough == #enemyTough - 2
     *      = -3/3 if #allyTough >= #enemyTough - 3 */
    score->toughMinionsScore = 0;
    int toughEnem = 0;
    int toughAllies = 0;
    foreach (Minion* m, *curPl->minions())
        if (m->hp() > 4)
            toughAllies += 1;
    foreach (Minion* m, *oppPl->minions())
        if (m->hp() > 4)
            toughEnem += 1;

    int toughDeltaQ = toughAllies - toughEnem;
    if (toughDeltaQ == 1)
        score->toughMinionsScore = 1./3;

    else if (toughDeltaQ == 2)
        score->toughMinionsScore = 2./3;

    else if (toughDeltaQ >= 3)
        score->toughMinionsScore = 1;

    else if (toughDeltaQ == -1)
        score->toughMinionsScore = -1./3;

    else if (toughDeltaQ == -2)
        score->toughMinionsScore = -2./3;

    else if (toughDeltaQ <= -3)
        score->toughMinionsScore = -1;


    /* Hand score = 0    if #allyHand == #enemyHand
     *            = 0.5  if #allyHand == #enemyHand + 1
     *            = 1    if #allyHand >= #enemyHand + 2
     *            = -0.5 if #allyHand == #enemyHand - 1
     *            = -1   if #allyHand <= #enemyHand - 2
     * */
    score->handScore = 0;
    int handDeltaQ = curPl->hand()->size() - oppPl->hand()->size();
    if (handDeltaQ == 1)
        score->handScore = 0.5;

    else if (handDeltaQ >= 2)
        score->handScore = 1;

    else if (handDeltaQ == -1)
        score->handScore = -0.5;

    else if (handDeltaQ <= -2)
        score->handScore = -1;


    /* Minions score = (#allyMinions - #enemyMinions) / (#allyMinions + #enemyMinions) */
    int enemMinionsQ = oppPl->minions()->size();
    int allyMinionsQ = curPl->minions()->size();
    score->minionsScore = ((float)(allyMinionsQ - enemMinionsQ)) / (allyMinionsQ + enemMinionsQ);
    if (allyMinionsQ + enemMinionsQ == 0)
        score->minionsScore = 0;

    /* Situation score */
    score->situationScore = 0;
    int bestTrade = TradeUtils::bestTrade(curPl, oppPl, false);

    // Defavorable trade for us: now score depends on the opponent best trade
    // (defavorable OR we can't attack! hence the <EQUAL)
    if (bestTrade <= 0) {
        bestTrade = TradeUtils::bestTrade(oppPl, curPl, true);

        // Defavorable trade for opponent -> score > 0
        if (bestTrade < 0)
            score->situationScore = -bestTrade;

        // Favorable trade for opponent -> score < 0
        else
            score->situationScore = -bestTrade;
    }

    // Favorable trade for us -> score > 0
    else
        score->situationScore = bestTrade;

    if (score->situationScore < -1)
        score->situationScore = -1;
    else if (score->situationScore > 1)
        score->situationScore = 1;

    /* Total score */
    score->score = score->TOUGH_MINIONS_MULT * score->toughMinionsScore +
                   score->HAND_MULT * score->handScore +
                   score->MINIONS_MULT * score->minionsScore +
                   score->SITUATION_MULT * score->situationScore;

    score->score /= score->TOUGH_MINIONS_MULT +
                    score->HAND_MULT +
                    score->MINIONS_MULT +
                    score->SITUATION_MULT;
    return score;
}

