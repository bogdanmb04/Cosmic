/**
 * @file GameEngine.hpp
 * @brief Concrete implementation of the game logic and state management.
 * @details This class manages the game loop, entity updates, collision detection,
 * and notifies listeners (UI) about state changes.
 */

#pragma once
#include "IGameEngine.hpp"
#include "Map.hpp"
#include "IGhost.hpp"
#include "GameConfig.hpp"
#include "GhostModeController.hpp"
#include "GameTypes/GameState.hpp"
#include "GameTypes/TileType.hpp" // Removed extra semicolon

#include <array>
#include <mutex>
#include <random>

namespace Pacman
{
    /**
     * @class GameEngine
     * @brief The central brain of the application.
     * @details Inherits from IGameEngine to provide a stable API for the GUI.
     * Handles thread safety using std::mutex since the UI and Logic might run on different threads.
     */
    class GameEngine : public IGameEngine
    {
    public:
        GameEngine();
        ~GameEngine() override = default;

        // --- IGameEngine Interface Implementation ---

        /**
         * @brief Resets the game to the initial state (Score 0, Lives 3, Level 1).
         */
        void StartNewGame() override;

        /**
         * @brief Advances the game simulation.
         * @param deltaTime Seconds elapsed since the last update.
         */
        void Update(float deltaTime) override;

        void SetPaused(bool isPaused) override;

        /**
         * @brief Queues a direction change for the player.
         * @note The direction is stored as 'desired' and applied only when the player reaches a valid intersection.
         */
        void SetPlayerDirection(Direction direction) override;

        // --- Getters ---
        GameState GetState() const override;
        PlayerState GetPlayerState() const override;
        Vector2 GetMapSize() const override;
        TileType GetTileAt(const Vector2& position) const override;
        std::vector<Vector2> GetPelletPositions() const override;
        int GetPelletCount() const override;
        std::vector<GhostState> GetGhostStates() const override;
        GhostMode GetGlobalGhostMode() const override;

        void AddListener(std::shared_ptr<IEventListener> listener) override;
        void RemoveListener(std::shared_ptr<IEventListener> listener) override;

        /**
         * @brief Static factory method to create an instance.
         */
        static std::shared_ptr<IGameEngine> CreateGameEngine();

    private:
        // --- Internal Logic Helpers ---

        /// @brief Loads the map and sets up initial variables.
        void InitializeGame();

        /// @brief Places the player at the start position.
        void InitializePlayer();

        /// @brief Full reset of player (including score/lives).
        void ResetPlayerForNewGame();

        /// @brief Resets ghosts to their home positions and assigns their strategies.
        void InitializeGhosts();

        /**
         * @brief Helper to calculate grid coordinates.
         * @param from Starting position.
         * @param dir Direction of movement.
         * @return The new coordinate (handles map wrapping/teleportation).
         */
        Vector2 GetNextPosition(const Vector2& from, Direction dir) const;

        /// @brief Checks if a tile is walkable for the player (not a Wall).
        bool CanWalk(const Vector2& from, Direction dir) const;

        /// @brief Checks if a tile is walkable for a ghost (can include Ghost Doors).
        bool CanGhostWalk(const Vector2& from, Direction dir, bool canUseGhostDoor = false) const;

        /// @brief Calculates ghost speed based on current game state (Frightened ghosts move slower).
        float GetCurrentGhostInterval() const;

        /// @brief Handles player movement logic and cornering.
        void UpdatePlayer();

        /**
         * @brief Handles interaction when player enters a tile.
         * @return true if something was eaten (Pellet/PowerPellet).
         */
        bool TryConsumeTile(const Vector2& pos);

        /// @brief Main loop for updating all ghosts.
        void UpdateGhosts();

        /// @brief Determines the specific target tile for a ghost based on its strategy.
        void CalculateGhostTarget(GhostState& ghost, size_t index, const Vector2& blinkyPos);

        /**
         * @brief AI Pathfinding logic.
         * @details Ghosts cannot turn back 180 degrees immediately. They look ahead
         * and choose the tile that minimizes distance to their target.
         */
        Direction ChooseGhostDirection(const GhostState& ghost);

        /// @brief Handling for "Eaten" ghosts (eyes) returning to the house.
        void UpdateEatenGhost(GhostState& ghost);

        /// @brief Forces all ghosts to reverse direction (used when modes change).
        void ReverseGhostDirections();

        /// @brief Checks if a position is inside the ghost house box.
        bool IsInOrNearGhostHouse(const Vector2& pos) const;

        /**
         * @brief Checks collision between Player and Ghosts.
         * @details Handles both dying (PAC-MAN eaten) and eating ghosts (Powered up).
         */
        void CheckCollisions();

        /// @brief Reduces lives and resets positions, or triggers Game Over.
        void HandlePlayerDeath();

        // --- Notification Helpers (Observer Pattern) ---
        void NotifyAll();
        void NotifyTileUpdated(const TileUpdate& update);
        void NotifyPlayerState();
        void NotifyGameState();
        void NotifyGhostsUpdated();
        void NotifyGhostModeChanged(GhostMode mode);

        // --- Member Variables ---
        mutable std::mt19937 rng_;                                  ///< Random number generator for Frightened mode.
        mutable std::mutex mutex_;                                  ///< Thread safety lock.
        std::vector<std::shared_ptr<IEventListener>> listeners_;    ///< List of UI observers.
        Map map_;                                                   ///< The grid data.
        GameState gameState_ = GameState::Paused;
        PlayerState playerState_{};
        Direction desiredDirection_ = Direction::None;              ///< Input buffer.
        std::array<GhostState, 4> ghostStates_;                     ///< Data for the 4 ghosts.
        std::array<std::unique_ptr<IGhost>, 4> ghostAIs_;           ///< Strategies for the 4 ghosts.
        GhostModeController modeController_;                        ///< Manages Scatter/Chase timers.
        int ghostsEatenThisPowerUp_ = 0;                            ///< Combo counter for score.
        float playerStepTimer_ = 0.0f;                              ///< Accumulator for player speed.
        float ghostStepTimer_ = 0.0f;                               ///< Accumulator for ghost speed.
    };
}