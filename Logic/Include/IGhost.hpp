/**
 * @file IGhost.hpp
 * @brief Defines the Strategy interface for Ghost AI.
 * @details Each ghost (Blinky, Pinky, Inky, Clyde) has a unique personality defined by
 * how it calculates its target tile during the "Chase" phase.
 */

#pragma once

#include <memory>
#include "GameTypes/Vector2.hpp"
#include "GameTypes/GhostState.hpp"
#include "GameTypes/PlayerState.hpp"

namespace Pacman {

    /**
     * @interface IGhost
     * @brief Interface for ghost targeting strategies.
     * @details Implements the Strategy Pattern. The game engine uses this interface
     * to request the next target, regardless of the specific ghost type.
     */
    class IGhost {
    public:
        virtual ~IGhost() = default;

        /**
         * @brief Calculates the specific target tile during Chase mode.
         * @details This logic varies for each ghost:
         * - Red: Targets player directly.
         * - Pink: Targets 4 tiles in front of player.
         * - Blue: Uses a vector based on Red's position and Player's position.
         * - Orange: Targets player, but retreats to scatter corner if too close.
         * * @param ghost The current state of this ghost.
         * @param player The current state of the player.
         * @param blinkyPosition The current position of the Red ghost (required specifically for Inky's logic).
         * @return The target grid coordinate.
         */
        virtual Vector2 CalculateChaseTarget(
            const GhostState& ghost,
            const PlayerState& player,
            const Vector2& blinkyPosition) const = 0;

        /**
         * @brief Gets the fixed "Scatter" target for the ghost.
         * @details In Scatter mode, each ghost circles a specific corner of the map.
         * @return The fixed corner coordinate.
         */
        virtual Vector2 GetScatterTarget() const = 0;

        /**
         * @brief Identifies which specific ghost personality this is.
         * @return The GhostType enum (Red, Pink, Blue, or Orange).
         */
        virtual GhostType GetGhostType() const = 0;
    };
}