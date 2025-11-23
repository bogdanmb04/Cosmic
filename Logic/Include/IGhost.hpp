#pragma once

#include "GameTypes.hpp"
#include <memory>

namespace Pacman {

    /// @brief Interface for ghost targeting strategies
    /// Each ghost type (Blinky, Pinky, Inky, Clyde) implements this differently
    class IGhost {
    public:
        virtual ~IGhost() = default;

        /// @brief Calculate chase mode target tile
        /// @param ghost Current ghost state
        /// @param player Current player state
        /// @param blinkyPosition Blinky's position (needed for Inky)
        virtual Vector2 CalculateChaseTarget(
            const GhostState& ghost,
            const PlayerState& player,
            const Vector2& blinkyPosition) const = 0;

        /// @brief Get scatter corner target
        virtual Vector2 GetScatterTarget() const = 0;

        /// @brief Get ghost type
        virtual GhostType GetGhostType() const = 0;
    };

    std::unique_ptr<IGhost> CreateRedAI();
    std::unique_ptr<IGhost> CreatePinkAI();
    std::unique_ptr<IGhost> CreateBlueAI();
    std::unique_ptr<IGhost> CreateOrangeAI();
    std::unique_ptr<IGhost> CreateGhostAI(GhostType type);

}