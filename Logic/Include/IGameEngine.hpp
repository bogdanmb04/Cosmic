#pragma once

#include "GameTypes.hpp"
#include "IEventListener.hpp"
#include <memory>
#include <vector>

namespace Pacman {

    /// @brief Main game engine interface
    class IGameEngine {
    public:
        virtual ~IGameEngine() = default;

        /// @brief Start a new game
        virtual void StartNewGame() = 0;

        /// @brief Update game logic
        /// @param deltaTime Time elapsed since last update (in seconds)
        virtual void Update(float deltaTime) = 0;

        /// @brief Set game paused state
        /// @param isPaused True to pause, false to resume
        virtual void SetPaused(bool isPaused) = 0;

        /// @brief Set the desired player direction
        /// @param direction The direction to move
        virtual void SetPlayerDirection(Direction direction) = 0;

        /// @brief Get the current game state
        /// @return Current game state
        virtual GameState GetState() const = 0;

        /// @brief Get the current player state
        /// @return Current player state
        virtual PlayerState GetPlayerState() const = 0;

        /// @brief Get map dimensions
        /// @return Map size as Vector2
        virtual Vector2 GetMapSize() const = 0;

        /// @brief Get tile type at specific position
        /// @param position The position to query
        /// @return Tile type at that position
        virtual TileType GetTileAt(const Vector2& position) const = 0;

        /// @brief Get all remaining pellet positions
        /// @return Vector of pellet positions
        virtual std::vector<Vector2> GetPelletPositions() const = 0;

        /// @brief Get current ghost states
        /// @return Vector of ghost states
        virtual std::vector<GhostState> GetGhostStates() const = 0;

        /// @brief Add an event listener
        /// @param listener Shared pointer to listener
        virtual void AddListener(std::shared_ptr<IEventListener> listener) = 0;

        /// @brief Remove an event listener
        /// @param listener Shared pointer to listener
        virtual void RemoveListener(std::shared_ptr<IEventListener> listener) = 0;
    };

    /// @brief Factory function to create game engine
    /// @return Shared pointer to game engine implementation
    std::shared_ptr<IGameEngine> CreateGameEngine();

}
