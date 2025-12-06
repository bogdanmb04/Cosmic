/**
 * @file IGameEngine.hpp
 * @brief Main interface of the game engine (API).
 * @details This abstract class hides the complex game logic (movement, collisions, AI)
 * from the graphical interface. The GUI only calls functions from here.
 */

#pragma once

#include "IEventListener.hpp"
#include <memory>
#include <vector>

namespace Pacman {

    /**
     * @interface IGameEngine
     * @brief Main interface of the game engine (API).
     * @details This abstract class acts as a facade, hiding the internal simulation details.
     */
    class IGameEngine {
    public:
        virtual ~IGameEngine() = default;

        /**
         * @brief Resets the map, score, and entity positions for a new game.
         */
        virtual void StartNewGame() = 0;

        /**
         * @brief Updates the game logic by one time step.
         * @param deltaTime Time elapsed (in seconds) since the last frame.
         * @note Must be called within the main application loop (Game Loop).
         */
        virtual void Update(float deltaTime) = 0;

        /**
         * @brief Pauses or resumes the game.
         * @param isPaused true to pause, false to resume.
         */
        virtual void SetPaused(bool isPaused) = 0;

        /**
         * @brief Submits the player's intent to change direction.
         * @param direction The desired direction (input from keyboard).
         * @note The direction does not change instantly if a wall exists;
         * the engine will wait until the turn is valid (cornering logic).
         */
        virtual void SetPlayerDirection(Direction direction) = 0;

        // --- Getters (for initial rendering / polling) ---

        /**
         * @brief Returns the current state of the game.
         * @return GameState (Running, Victory, etc.)
         */
        virtual GameState GetState() const = 0;

        /// @brief Returns current player data (position, score, lives).
        virtual PlayerState GetPlayerState() const = 0;

        /// @brief Returns map dimensions (Width x Height).
        virtual Vector2 GetMapSize() const = 0;

        /// @brief Gets the type of tile at a specific coordinate (Wall, Pellet, etc.).
        virtual TileType GetTileAt(const Vector2& position) const = 0;

        /// @brief Gets a list of all positions where pellets currently exist (for rendering).
        virtual std::vector<Vector2> GetPelletPositions() const = 0;

        /// @brief Returns the count of remaining pellets on the map.
        virtual int GetPelletCount() const = 0;

        /// @brief Returns the state of all 4 ghosts.
        virtual std::vector<GhostState> GetGhostStates() const = 0;

        /// @brief Returns the current global ghost mode (Chase/Scatter/Frightened).
        virtual GhostMode GetGlobalGhostMode() const = 0;

        // --- Event System (Observer Pattern) ---

        /**
         * @brief Registers a listener for game events.
         * @param listener Pointer to the object that wants to receive notifications (usually the GUI).
         */
        virtual void AddListener(std::shared_ptr<IEventListener> listener) = 0;

        /**
         * @brief Unsubscribes a listener from game events.
         */
        virtual void RemoveListener(std::shared_ptr<IEventListener> listener) = 0;
    };

    /**
     * @brief Factory method for creating the game engine instance.
     * @return A shared_ptr to the concrete implementation of IGameEngine.
     */
    std::shared_ptr<IGameEngine> CreateGameEngine();

}