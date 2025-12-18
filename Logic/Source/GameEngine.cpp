/**
 * @file GameEngine.cpp
 * @brief Implementation of the core game loop and logic.
 * @details Handles the real-time updates, collision detection, and AI state management.
 */

#include "GameEngine.hpp"
#include "Ghost.hpp"
#include "RedStrategy.hpp"
#include "BlueStrategy.hpp"
#include "PinkStrategy.hpp"
#include "OrangeStrategy.hpp"

#include <algorithm>
#include <mutex>
#include <array>
#include <random>
#include <climits>

namespace Pacman
{
    /**
     * @brief Constructor.
     * @details Initializes the 4 specific ghost personalities using Dependency Injection.
     * Each Ghost object receives a specific Strategy (Red, Pink, etc.) and a Type identifier.
     */
    GameEngine::GameEngine()
    {
        // Inject specific behaviors into the generic Ghost class
        ghostAIs_[0] = std::make_unique<Ghost>(std::make_unique<RedStrategy>(), GhostType::Red);
        ghostAIs_[1] = std::make_unique<Ghost>(std::make_unique<PinkStrategy>(), GhostType::Pink);
        ghostAIs_[2] = std::make_unique<Ghost>(std::make_unique<BlueStrategy>(), GhostType::Blue);
        ghostAIs_[3] = std::make_unique<Ghost>(std::make_unique<OrangeStrategy>(), GhostType::Orange);

        rng_.seed(std::random_device{}());
        InitializeGame();
    }

    void GameEngine::StartNewGame()
    {
        std::lock_guard<std::mutex> lock(mutex_); // Ensure thread safety
        map_.Initialize();
        ResetPlayerForNewGame();
        InitializeGhosts();
        modeController_.Reset();

        // Reset timers
        playerStepTimer_ = 0.0f;
        ghostStepTimer_ = 0.0f;
        ghostsEatenThisPowerUp_ = 0;

        gameState_ = GameState::Running;
        NotifyAll();
    }

    /**
     * @brief The main game loop update.
     * @details Performs the following steps:
     * 1. Updates Ghost Mode timers (Scatter/Chase/Frightened).
     * 2. Checks for mode changes to reverse ghost directions.
     * 3. Updates Player position based on accumulated time (fixed step).
     * 4. Updates Ghost positions based on their specific speed interval.
     * 5. Checks for collisions and Win condition.
     */
    void GameEngine::Update(float deltaTime)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (gameState_ != GameState::Running) return;

        // --- 1. Update Ghost AI Modes ---
        GhostMode previousMode = modeController_.GetCurrentMode();
        modeController_.Update(deltaTime);
        GhostMode currentMode = modeController_.GetCurrentMode();

        // --- 2. Handle Mode Changes ---
        // Ghosts must reverse direction immediately when switching Scatter <-> Chase
        if (modeController_.ShouldReverseDirection()) {
            ReverseGhostDirections();
            NotifyGhostModeChanged(currentMode);
        }

        // Handle Frightened mode expiration
        if (previousMode == GhostMode::Frightened && currentMode != GhostMode::Frightened) {
            for (auto& ghost : ghostStates_) {
                ghost.IsFrightened = false;
            }
            ghostsEatenThisPowerUp_ = 0;
        }

        // --- 3. Update Player Movement ---
        // Accumulate time to handle variable frame rates
        playerStepTimer_ += deltaTime;
        while (playerStepTimer_ >= GameConfig::PlayerStepInterval) {
            UpdatePlayer();
            playerStepTimer_ -= GameConfig::PlayerStepInterval;
        }

        // --- 4. Update Ghost Movement ---
        // Ghosts might move slower if Frightened or depending on level difficulty
        float ghostInterval = GetCurrentGhostInterval();
        ghostStepTimer_ += deltaTime;
        while (ghostStepTimer_ >= ghostInterval) {
            UpdateGhosts();
            ghostStepTimer_ -= ghostInterval;
        }

        // --- 5. Collision & Win Check ---
        CheckCollisions();

        if (map_.GetPelletCount() == 0) {
            gameState_ = GameState::Victory;
            NotifyGameState();
        }
    }

    void GameEngine::SetPaused(bool isPaused)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (gameState_ == GameState::GameOver || gameState_ == GameState::Victory) return;
        gameState_ = isPaused ? GameState::Paused : GameState::Running;
        NotifyGameState();
    }

    void GameEngine::SetPlayerDirection(Direction direction)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        desiredDirection_ = direction; // Store intent; applied in UpdatePlayer()
    }

    // --- Getters ---

    GameState GameEngine::GetState() const { return gameState_; }
    PlayerState GameEngine::GetPlayerState() const { return playerState_; }
    Vector2 GameEngine::GetMapSize() const { return map_.GetSize(); }
    TileType GameEngine::GetTileAt(const Vector2& position) const { return map_.GetTileAt(position); }
    std::vector<Vector2> GameEngine::GetPelletPositions() const { return map_.GetPelletPositions(); }
    int GameEngine::GetPelletCount() const { return map_.GetPelletCount(); }

    std::vector<GhostState> GameEngine::GetGhostStates() const {
        return std::vector<GhostState>(ghostStates_.begin(), ghostStates_.end());
    }

    GhostMode GameEngine::GetGlobalGhostMode() const { return modeController_.GetCurrentMode(); }

    // --- Observer Management ---

    void GameEngine::AddListener(std::shared_ptr<IEventListener> listener)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.push_back(listener);
    }

    void GameEngine::RemoveListener(std::shared_ptr<IEventListener> listener)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
    }

    std::shared_ptr<IGameEngine> GameEngine::CreateGameEngine()
    {
        return std::make_shared<GameEngine>();
    }

    // --- Internal Logic ---

    void GameEngine::InitializeGame()
    {
        map_.Initialize();
        ResetPlayerForNewGame();
        InitializeGhosts();
        modeController_.Reset();
        gameState_ = GameState::Paused;
    }

    void GameEngine::InitializePlayer()
    {
        playerState_.Position = { GameConfig::PlayerStartX, GameConfig::PlayerStartY };
        playerState_.CurrentDirection = Direction::Left;
        playerState_.IsPoweredUp = false;
        desiredDirection_ = Direction::Left;
    }

    void GameEngine::ResetPlayerForNewGame()
    {
        InitializePlayer();
        playerState_.Score = 0;
        playerState_.Lives = GameConfig::StartingLives;
    }

    void GameEngine::InitializeGhosts()
    {
        // Define starting positions and behaviors for the 4 ghosts
        // Red (Blinky) starts outside the house
        ghostStates_[0] = GhostState{
            {13, 11}, Direction::Left, false, false,
            GhostType::Red, GhostMode::Scatter, "Red",
            {0, 0}, ghostAIs_[0]->GetScatterTarget()
        };
        // Pink (Pinky) starts inside
        ghostStates_[1] = GhostState{
            {13, 14}, Direction::Down, false, false,
            GhostType::Pink, GhostMode::Scatter, "Pink",
            {0, 0}, ghostAIs_[1]->GetScatterTarget()
        };
        // Blue (Inky) starts inside
        ghostStates_[2] = GhostState{
            {11, 14}, Direction::Up, false, false,
            GhostType::Blue, GhostMode::Scatter, "Blue",
            {0, 0}, ghostAIs_[2]->GetScatterTarget()
        };
        // Orange (Clyde) starts inside
        ghostStates_[3] = GhostState{
            {15, 14}, Direction::Up, false, false,
            GhostType::Orange, GhostMode::Scatter, "Orange",
            {0, 0}, ghostAIs_[3]->GetScatterTarget()
        };
    }

    Vector2 GameEngine::GetNextPosition(const Vector2& from, Direction dir) const
    {
        Vector2 delta = GetDirectionDelta(dir);
        Vector2 next = { from.X + delta.X, from.Y + delta.Y };
        return map_.WrapPosition(next); // Handles wrapping around screen edges
    }

    bool GameEngine::CanWalk(const Vector2& from, Direction dir) const
    {
        return map_.IsWalkable(GetNextPosition(from, dir));
    }

    bool GameEngine::CanGhostWalk(const Vector2& from, Direction dir, bool canUseGhostDoor) const
    {
        return map_.IsGhostWalkable(GetNextPosition(from, dir), canUseGhostDoor);
    }

    float GameEngine::GetCurrentGhostInterval() const
    {
        if (modeController_.GetCurrentMode() == GhostMode::Frightened) {
            return GameConfig::GhostFrightenedStepInterval; // Move slower when frightened
        }

        // "Cruise Elroy" logic: Blinky speeds up when pellets remain few
        int pellets = map_.GetPelletCount();
        if (pellets <= GameConfig::ElroyDotsThreshold2) {
            return GameConfig::GhostStepInterval * 0.8f; // 20% faster
        }
        else if (pellets <= GameConfig::ElroyDotsThreshold1) {
            return GameConfig::GhostStepInterval * 0.9f; // 10% faster
        }
        return GameConfig::GhostStepInterval;
    }

    void GameEngine::UpdatePlayer()
    {
        // Try to turn in the desired direction
        if (desiredDirection_ != Direction::None && CanWalk(playerState_.Position, desiredDirection_)) {
            playerState_.CurrentDirection = desiredDirection_;
        }

        // Move in current direction if possible
        if (playerState_.CurrentDirection != Direction::None &&
            CanWalk(playerState_.Position, playerState_.CurrentDirection)) {
            playerState_.Position = GetNextPosition(playerState_.Position, playerState_.CurrentDirection);

            // Check for tile interactions (eat pellets, etc.)
            TryConsumeTile(playerState_.Position);
            NotifyPlayerState();
        }
    }

    /**
     * @brief Handles tile consumption logic.
     * @return true if a pellet or power pellet was consumed.
     */
    bool GameEngine::TryConsumeTile(const Vector2& pos)
    {
        TileType tile = map_.GetTileAt(pos);

        if (tile == TileType::Pellet) {
            map_.SetTileAt(pos, TileType::Path);
            playerState_.Score += GameConfig::PelletScore;
            NotifyTileUpdated({ pos, TileType::Path });
            return true;
        }

        if (tile == TileType::PowerPellet) {
            map_.SetTileAt(pos, TileType::Path);
            playerState_.Score += GameConfig::PowerPelletScore;

            // Trigger global Frightened mode
            modeController_.TriggerFrightenedMode(GameConfig::PowerUpDuration);
            playerState_.IsPoweredUp = true;
            ghostsEatenThisPowerUp_ = 0;

            // Reverse all non-eaten ghosts
            for (auto& ghost : ghostStates_) {
                if (!ghost.IsEaten) {
                    ghost.IsFrightened = true;
                    ghost.CurrentDirection = GetOppositeDirection(ghost.CurrentDirection);
                }
            }

            NotifyTileUpdated({ pos, TileType::Path });
            NotifyGhostsUpdated();
            NotifyGhostModeChanged(GhostMode::Frightened);
            return true;
        }
        return false;
    }

    void GameEngine::UpdateGhosts()
    {
        Vector2 blinkyPos = ghostStates_[0].Position; // Needed for Inky's AI

        for (size_t i = 0; i < ghostStates_.size(); ++i)
        {
            GhostState& ghost = ghostStates_[i];

            // If eaten (eyes only), rush back to ghost house
            if (ghost.IsEaten) {
                UpdateEatenGhost(ghost);
                continue;
            }

            // Standard movement
            CalculateGhostTarget(ghost, i, blinkyPos);
            Direction newDir = ChooseGhostDirection(ghost);

            if (newDir != Direction::None) {
                ghost.CurrentDirection = newDir;
            }

            if (ghost.CurrentDirection != Direction::None) {
                bool canUseGhostDoor = IsInOrNearGhostHouse(ghost.Position);
                if (CanGhostWalk(ghost.Position, ghost.CurrentDirection, canUseGhostDoor)) {
                    ghost.Position = GetNextPosition(ghost.Position, ghost.CurrentDirection);
                }
            }
        }
        NotifyGhostsUpdated();
    }

    void GameEngine::CalculateGhostTarget(GhostState& ghost, size_t index, const Vector2& blinkyPos)
    {
        GhostMode globalMode = modeController_.GetCurrentMode();

        if (ghost.IsFrightened) {
            ghost.TargetTile = ghost.Position; // Target doesn't matter for random movement
            ghost.Mode = GhostMode::Frightened;
            return;
        }

        if (globalMode == GhostMode::Scatter) {
            ghost.TargetTile = ghost.ScatterTarget;
            ghost.Mode = GhostMode::Scatter;
        }
        else {
            // Delegate chase logic to the specific Strategy implementation
            ghost.TargetTile = ghostAIs_[index]->CalculateChaseTarget(ghost, playerState_, blinkyPos);
            ghost.Mode = GhostMode::Chase;
        }
    }

    /**
     * @brief Determines the best direction for a ghost to move.
     * @details Ghosts look one tile ahead. They prioritize minimizing the Euclidean distance
     * squared to their target. They cannot reverse direction immediately (180 turns disallowed).
     *
     * Priority order in case of ties: Up > Left > Down > Right.
     */
    Direction GameEngine::ChooseGhostDirection(const GhostState& ghost)
    {
        Direction opposite = GetOppositeDirection(ghost.CurrentDirection);
        std::array<Direction, 4> priorities = {
            Direction::Up, Direction::Left, Direction::Down, Direction::Right
        };

        Direction bestDir = Direction::None;
        int bestDistSq = INT_MAX;
        bool canUseGhostDoor = IsInOrNearGhostHouse(ghost.Position);

        for (Direction dir : priorities) {
            // Rule: Cannot reverse direction
            if (dir == opposite) continue;
            // Rule: Must be walkable
            if (!CanGhostWalk(ghost.Position, dir, canUseGhostDoor)) continue;

            if (ghost.IsFrightened) {
                // Random direction when frightened
                std::uniform_int_distribution<int> dist(0, 3);
                if (dist(rng_) == 0 || bestDir == Direction::None) {
                    bestDir = dir;
                }
            }
            else {
                // Pathfinding: Choose neighbor with smallest distance to target
                Vector2 nextPos = GetNextPosition(ghost.Position, dir);
                int distSq = nextPos.DistanceSquared(ghost.TargetTile);
                if (distSq < bestDistSq) {
                    bestDistSq = distSq;
                    bestDir = dir;
                }
            }
        }
        return bestDir;
    }

    void GameEngine::UpdateEatenGhost(GhostState& ghost)
    {
        Vector2 houseTarget = { GameConfig::GhostHouseX, GameConfig::GhostHouseDoorY };

        // Check if returned home
        if (ghost.Position == houseTarget ||
            (ghost.Position.Y >= GameConfig::GhostHouseDoorY &&
                std::abs(ghost.Position.X - GameConfig::GhostHouseX) <= 2)) {

            ghost.IsEaten = false;
            ghost.IsFrightened = modeController_.IsFrightened(); // Inherit current mode
            ghost.Position = { GameConfig::GhostHouseX, GameConfig::GhostHouseY }; // Respawn inside
            return;
        }

        // Simple pathfinding back to house (ignores "No reverse" rule slightly to ensure return)
        Direction opposite = GetOppositeDirection(ghost.CurrentDirection);
        std::array<Direction, 4> priorities = {
            Direction::Up, Direction::Left, Direction::Down, Direction::Right
        };

        Direction bestDir = Direction::None;
        int bestDistSq = INT_MAX;

        for (Direction dir : priorities) {
            if (dir == opposite && bestDir != Direction::None) continue;
            if (!CanGhostWalk(ghost.Position, dir, true)) continue; // Can enter ghost doors

            Vector2 nextPos = GetNextPosition(ghost.Position, dir);
            int distSq = nextPos.DistanceSquared(houseTarget);
            if (distSq < bestDistSq) {
                bestDistSq = distSq;
                bestDir = dir;
            }
        }

        if (bestDir != Direction::None) {
            ghost.CurrentDirection = bestDir;
            ghost.Position = GetNextPosition(ghost.Position, bestDir);
        }
    }

    void GameEngine::ReverseGhostDirections()
    {
        for (auto& ghost : ghostStates_)
        {
            if (!ghost.IsEaten)
            {
                ghost.CurrentDirection = GetOppositeDirection(ghost.CurrentDirection);
            }
        }
    }

    bool GameEngine::IsInOrNearGhostHouse(const Vector2& pos) const
    {
        return pos.Y >= 12 && pos.Y <= 16 && pos.X >= 10 && pos.X <= 17;
    }

    void GameEngine::CheckCollisions()
    {
        for (auto& ghost : ghostStates_)
        {
            if (ghost.Position == playerState_.Position)
            {
                if (ghost.IsEaten)
                    continue;

                if (ghost.IsFrightened)
                {
                    // Player eats Ghost
                    ghost.IsEaten = true;
                    ghost.IsFrightened = false;

                    // Award points (200, 400, 800, 1600...)
                    // Simplified here to constant for now, or based on counter
                    playerState_.Score += GameConfig::GhostScore;
                    ghostsEatenThisPowerUp_++;

                    NotifyPlayerState();
                    NotifyGhostsUpdated();
                }
                else
                {
                    // Ghost eats Player
                    HandlePlayerDeath();
                    return;
                }
            }
        }
    }

    void GameEngine::HandlePlayerDeath()
    {
        playerState_.Lives--;
        NotifyPlayerState();

        if (playerState_.Lives <= 0)
        {
            gameState_ = GameState::GameOver;
            NotifyGameState();
        }
        else
        {
            // Soft reset (keep score and level progress)
            InitializePlayer();
            InitializeGhosts();
            modeController_.Reset();
            playerStepTimer_ = 0.0f;
            ghostStepTimer_ = 0.0f;
            NotifyGhostsUpdated();
        }
    }

    void GameEngine::NotifyAll()
    {
        NotifyGameState();
        NotifyPlayerState();
        NotifyGhostsUpdated();
    }

    void GameEngine::NotifyTileUpdated(const TileUpdate& update)
    {
        for (auto& l : listeners_) if (l) l->OnTileUpdated(update);
    }

    void GameEngine::NotifyPlayerState()
    {
        playerState_.IsPoweredUp = modeController_.IsFrightened();
        for (auto& l : listeners_) if (l) l->OnPlayerStateChanged(playerState_);
    }

    void GameEngine::NotifyGameState()
    {
        for (auto& l : listeners_) if (l) l->OnGameStateChanged(gameState_);
    }

    void GameEngine::NotifyGhostsUpdated()
    {
        std::vector<GhostState> states(ghostStates_.begin(), ghostStates_.end());
        for (auto& l : listeners_) if (l) l->OnGhostsUpdated(states);
    }

    void GameEngine::NotifyGhostModeChanged(GhostMode mode)
    {
        for (auto& l : listeners_) if (l) l->OnGhostModeChanged(mode);
    }
}