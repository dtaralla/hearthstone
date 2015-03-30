#include "playaction.h"
#include "card.h"
#include "player.h"
#include "cardinfo.h"
#include "minion.h"
#include "iorequest.h"
#include "dbouput.h"
#include "game.h"

PlayAction::PlayAction(Card* c) :
    Action()
{
    m_card = c;
}

PlayAction::~PlayAction()
{

}



QString PlayAction::toString() const
{
    return "Play " + m_card->name();
}

ActionType PlayAction::type() const
{
    return ActionTypes::PLAY_CARD;
}

void PlayAction::resolve(const Event* e)
{
    Q_ASSERT(resolvable(e));
    Game* game = m_card->owner()->game();

    // Remove card from hand and pay the mana cost
    m_card->owner()->modifyCurrentMana(-m_card->effectiveManaCost());
    m_card->owner()->removeFromHand(m_card);

    if (m_card->type() == CardTypes::CARD_MINION)
        ((Minion*) m_card)->setBattlecryResolved(false);

    Event ev(Event::BEFORE_CARD_PLAYED);
    ev.putExtra<Card>("card", m_card);
    game->trigger(ev);

    // Execute atomically the consequence of playing m_card
    foreach (Action* a, *m_card->playActionConsequences())
        if (Game::IsDBGenerationModeOn() && a->isTargetedAction()) {
            DBOutput::Instance(game)->buffer(game->environment(), a);
            a->resolve();
            Game::BoardControlScore* score = game->meta_BoardControlScore();
            DBOutput::Instance(game)->buffer(score->score, a);
            delete score;
        }
        else
            a->resolve();

    // Handle battlecry if any
    if (m_card->type() == CardTypes::CARD_MINION) {
        Minion* m = (Minion*) m_card;
        foreach (Action* a, *m->battlecry()) {
            IORequest* ir = new IORequest(m_card->owner(), IORequest::BATTLECRY);
            ir->putExtra<Minion>("minion", m);
            ir->putExtra<Action>("action", a);
            emit m_card->owner()->battlecry(ir);

            if (a->resolvable(e)) {
                if (Game::IsDBGenerationModeOn() && a->isTargetedAction()) {
                    DBOutput::Instance(game)->buffer(game->environment(), a);
                    a->resolve();
                    Game::BoardControlScore* score = game->meta_BoardControlScore();
                    DBOutput::Instance(game)->buffer(score->score, a);
                    delete score;
                }
                else
                    a->resolve();
            }
        }
        m->setBattlecryResolved(true);
    }
    else if (m_card->type() == CardTypes::CARD_SPELL) {
        m_card->owner()->putCardInGraveyard(m_card);
    }
}

Action* PlayAction::cloneFlyweight() const
{
    return new PlayAction(m_card);
}

bool PlayAction::resolvable(const Event* e) const
{
    if (m_card->owner()->currentMana() < m_card->effectiveManaCost())
        return false;

    foreach (Action* a, *m_card->playActionConsequences())
        if (!a->resolvable(e))
            return false;

    return true;
}

Action* PlayAction::setSourceCard(Card* c) const
{
    qCritical() << "Not meant to be called; card was" << *c->base();
    return NULL;
}

