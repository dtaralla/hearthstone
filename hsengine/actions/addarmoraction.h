#ifndef ADDARMORACTION_H
#define ADDARMORACTION_H

#include "action.h"

/**
 * @author David Taralla
 * @date 2015
 *
 * @brief The AddArmorAction class represents an Action whose effect is to
 * increase (or decrease) the armor of heroes.
 *
 * User-selection of the hero is not possible. The hero whose armor has to be
 * modified is determined at runtime, thanks to an Owner qualifier like
 * @li Owners::ALLY : The target will be the allied hero upon calling resolve()
 * @li Owners::ENEMY : The target will be the enemy hero upon calling resolve()
 * @li Owners::ANY_OWNER : Both heroes will get the effect
 *
 * The quantity of armor point to give or remove is a second parameter for this
 * action.
 *
 * @ingroup cardActions
 */
class AddArmorAction : public Action
{
public:
    /**
     * @brief Constructor.
     *
     * @param quantity The quantity of armor points to add (or remove, if
     * negative).
     *
     * @param target The relative Owner of the hero whose armor should be
     * changed.
     *
     * @sa Owners
     */
    AddArmorAction(int quantity, Owner target);

    /**
     * @brief Gets the relative Owner of the hero whose armor
     * should be changed.
     *
     * The hero whose armor has to be modified is determined at runtime, thanks
     * to an Owner qualifier like
     * @li Owners::ALLY : The target will be the allied hero upon calling
     * resolve()
     * @li Owners::ENEMY : The target will be the enemy hero upon calling
     * resolve()
     * @li Owners::ANY_OWNER : Both heroes will get the effect
     *
     * @return The relative Owner
     *
     * @sa Owners
     */
    Owner targetOwner() const;

    /**
     * @brief Gets the quantity of armor points to add (or remove).
     *
     * @return The armor modifier to apply. If negative, armor points are
     * removed instead of added. The armor however cannot go below 0.
     *
     * @sa Hero::addArmor()
     */
    int quantity() const;

private:
    /**
     * @brief The quantity of armor points to add (or remove).
     *
     * If negative, armor points will be removed instead of added. The armor
     * however cannot go below 0.
     *
     * @sa quantity()
     */
    int m_quantity;

    /**
     * @brief The relative Owner of the hero whose armor should
     * be changed.
     *
     * @sa targetOwner(), Owners
     */
    Owner m_targetOwner;

    // Stringable interface
public:
    QString toString() const;

    // Action interface
public:
    /**
     * @brief This action type.
     *
     * @return ActionTypes::ADD_ARMOR
     *
     * @sa ActionTypes
     */
    ActionType type() const;

    void resolve(const Event* e = NULL);
    AddArmorAction* cloneFlyweight() const;
};

#endif // ADDARMORACTION_H
