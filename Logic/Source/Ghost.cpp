/**
 * @file Ghost.cpp
 * @brief Implementation of the Ghost class methods.
 */

#include "IGhost.hpp"
#include "GameConfig.hpp"
#include "Ghost.hpp"

namespace Pacman
{
    /**
     * @brief Initializes the ghost with a specific strategy and type.
     * @details Uses std::move to take ownership of the unique_ptr strategy.
     */
    Ghost::Ghost(std::unique_ptr<IGhostStrategy> ghostStrategy, GhostType type)
        : ghostStrategy_{ std::move(ghostStrategy) }
        , type_{ type }
    {
        // EMPTY
    }

    /**
     * @brief Delegates the target calculation to the strategy.
     * @return The target grid coordinate calculated by the specific AI strategy.
     */
    Vector2 Ghost::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
    {
        return ghostStrategy_->CalculateChaseTarget(ghost, player, blinkyPosition);
    }

    /**
     * @brief Gets the scatter target from the strategy.
     */
    Vector2 Ghost::GetScatterTarget() const
    {
        return ghostStrategy_->GetScatterTarget();
    }

    /**
     * @brief Returns the stored ghost type.
     */
    GhostType Ghost::GetGhostType() const
    {
        return type_;
    }
}