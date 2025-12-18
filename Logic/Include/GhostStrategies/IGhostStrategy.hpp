/**
 * @file IGhostStrategy.hpp
 * @brief Interface for ghost movement algorithms.
 * @details Implements the Strategy Pattern, allowing different ghosts (Red, Pink, etc.)
 * to have different target calculation logic while sharing the same movement code.
 */

#pragma once
#include "GameTypes/Vector2.hpp"
#include "GameTypes/PlayerState.hpp"
#include "GameTypes/GhostState.hpp"

namespace Pacman
{
    /**
     * @interface IGhostStrategy
     * @brief Abstract base class for ghost AI behaviors.
     */
    class IGhostStrategy
    {
    public:
        virtual ~IGhostStrategy() = default;

        /**
         * @brief Calculates the target tile for the ghost during Chase mode.
         * @param ghost The current state of the ghost (position, direction).
         * @param player The current state of the player (position, direction).
         * @param blinkyPosition The position of the Red ghost (required for Inky's vector calculation).
         * @return The target grid coordinate.
         */
        virtual Vector2 CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const = 0;

        /**
         * @brief Gets the fixed target tile for Scatter mode.
         * @details Each ghost has a specific corner they circle around during Scatter mode.
         * @return The fixed grid coordinate of the scatter corner.
         */
        virtual Vector2 GetScatterTarget() const = 0;
    };
}