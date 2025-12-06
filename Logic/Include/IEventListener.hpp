#pragma once
#include "TileUpdate.hpp"
#include "PlayerState.hpp"
#include "GameState.hpp"
#include "GhostState.hpp"
#include <vector>

namespace Pacman {

    /// @brief Interface for objects that want to receive game state updates
    class IEventListener {
    public:
        virtual ~IEventListener() = default;

        /// @brief Called when a tile is updated (e.g., pellet eaten)
        /// @param update The tile update information
        virtual void OnTileUpdated(const TileUpdate& update) = 0;

        /// @brief Called when the player state changes
        /// @param state The new player state
        virtual void OnPlayerStateChanged(const PlayerState& state) = 0;

        /// @brief Called when the game state changes
        /// @param state The new game state
        virtual void OnGameStateChanged(GameState state) = 0;

        /// @brief Called when ghost positions/states update
        /// @param ghosts The current ghost states
        virtual void OnGhostsUpdated(const std::vector<GhostState>& ghosts) = 0;

        virtual void OnGhostModeChanged(GhostMode mode) {}
    };

}
