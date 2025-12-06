/**
 * @file IEventListener.hpp
 * @brief Observer interface for handling game events.
 * @details Implement this interface in your GUI class to react to changes in the game engine
 * without needing to poll the engine every frame.
 */

#pragma once
#include "GameTypes/TileUpdate.hpp"
#include "GameTypes/PlayerState.hpp"
#include "GameTypes/GameState.hpp"
#include "GameTypes/GhostState.hpp"
#include <vector>

namespace Pacman {

    /**
     * @interface IEventListener
     * @brief Interface for objects that want to receive game state updates.
     * @details This allows the separation of Game Logic (Engine) from the Presentation Layer (UI/Render).
     */
    class IEventListener {
    public:
        virtual ~IEventListener() = default;

        /**
         * @brief Called when a single map tile is updated.
         * @details Optimization: This allows the UI to redraw only the specific modified tile
         * (e.g., when a pellet is eaten) instead of the entire map.
         * @param update Structure containing the position and the new type of the tile.
         */
        virtual void OnTileUpdated(const TileUpdate& update) = 0;

        /**
         * @brief Called when player data changes (Position, Score, Lives).
         * @param state The new complete state of the player.
         */
        virtual void OnPlayerStateChanged(const PlayerState& state) = 0;

        /**
         * @brief Called when the global game flow state changes.
         * @param state The new state (e.g., transitioning from Running to Victory).
         */
        virtual void OnGameStateChanged(GameState state) = 0;

        /**
         * @brief Called every frame when ghost positions or animations update.
         * @param ghosts A vector containing the current state of all ghosts.
         */
        virtual void OnGhostsUpdated(const std::vector<GhostState>& ghosts) = 0;

        /**
         * @brief Called when the ghost AI behavior mode changes.
         * @details Useful for triggering visual cues (e.g., ghosts turning blue) or sound effects.
         * @param mode The new active mode (Chase, Scatter, or Frightened).
         */
        virtual void OnGhostModeChanged(GhostMode mode) {}
    };

}