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


Pacman::GameEngine::GameEngine()
{
    ghostAIs_[0] = std::make_unique<Ghost>(std::make_unique<RedStrategy>(), GhostType::Red);
    ghostAIs_[1] = std::make_unique<Ghost>(std::make_unique<PinkStrategy>(), GhostType::Pink);
    ghostAIs_[2] = std::make_unique<Ghost>(std::make_unique<BlueStrategy>(), GhostType::Blue);
    ghostAIs_[3] = std::make_unique<Ghost>(std::make_unique<OrangeStrategy>(), GhostType::Orange);;
    rng_.seed(std::random_device{}());
    InitializeGame();
}

void Pacman::GameEngine::StartNewGame()
{
    std::lock_guard<std::mutex> lock(mutex_);
    map_.Initialize();
    ResetPlayerForNewGame();
    InitializeGhosts();
    modeController_.Reset();
    playerStepTimer_ = 0.0f;
    ghostStepTimer_ = 0.0f;
    ghostsEatenThisPowerUp_ = 0;
    gameState_ = GameState::Running;
    NotifyAll();
}

void Pacman::GameEngine::Update(float deltaTime)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (gameState_ != GameState::Running) return;

    // Update ghost mode timing
    GhostMode previousMode = modeController_.GetCurrentMode();
    modeController_.Update(deltaTime);
    GhostMode currentMode = modeController_.GetCurrentMode();

    // Ghosts reverse direction on mode change
    if (modeController_.ShouldReverseDirection()) {
        ReverseGhostDirections();
        NotifyGhostModeChanged(currentMode);
    }

    // Handle frightened mode ending
    if (previousMode == GhostMode::Frightened && currentMode != GhostMode::Frightened) {
        for (auto& ghost : ghostStates_) {
            ghost.IsFrightened = false;
        }
        ghostsEatenThisPowerUp_ = 0;
    }

    // Update player
    playerStepTimer_ += deltaTime;
    while (playerStepTimer_ >= GameConfig::PlayerStepInterval) {
        UpdatePlayer();
        playerStepTimer_ -= GameConfig::PlayerStepInterval;
    }

    // Update ghosts
    float ghostInterval = GetCurrentGhostInterval();
    ghostStepTimer_ += deltaTime;
    while (ghostStepTimer_ >= ghostInterval) {
        UpdateGhosts();
        ghostStepTimer_ -= ghostInterval;
    }

    CheckCollisions();

    // Win condition
    if (map_.GetPelletCount() == 0) {
        gameState_ = GameState::Victory;
        NotifyGameState();
    }
}

void Pacman::GameEngine::SetPaused(bool isPaused)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (gameState_ == GameState::GameOver || gameState_ == GameState::Victory) return;
    gameState_ = isPaused ? GameState::Paused : GameState::Running;
    NotifyGameState();
}

void Pacman::GameEngine::SetPlayerDirection(Direction direction)
{
    std::lock_guard<std::mutex> lock(mutex_);
    desiredDirection_ = direction;
}

Pacman::GameState Pacman::GameEngine::GetState() const
{
    return gameState_;
}

Pacman::PlayerState Pacman::GameEngine::GetPlayerState() const
{
    return playerState_;
}

Pacman::Vector2 Pacman::GameEngine::GetMapSize() const
{
    return map_.GetSize();
}

Pacman::TileType Pacman::GameEngine::GetTileAt(const Vector2& position) const
{
    return map_.GetTileAt(position);
}

std::vector<Pacman::Vector2> Pacman::GameEngine::GetPelletPositions() const
{
    return map_.GetPelletPositions();
}

int Pacman::GameEngine::GetPelletCount() const
{
    return map_.GetPelletCount();
}

std::vector<Pacman::GhostState> Pacman::GameEngine::GetGhostStates() const
{
    return std::vector<GhostState>(ghostStates_.begin(), ghostStates_.end());
}

Pacman::GhostMode Pacman::GameEngine::GetGlobalGhostMode() const
{
    return modeController_.GetCurrentMode();
}

void Pacman::GameEngine::AddListener(std::shared_ptr<IEventListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.push_back(listener);
}

void Pacman::GameEngine::RemoveListener(std::shared_ptr<IEventListener> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
}

std::shared_ptr<Pacman::IGameEngine> Pacman::GameEngine::CreateGameEngine()
{
    return std::make_shared<GameEngine>();
}

void Pacman::GameEngine::InitializeGame()
{
    map_.Initialize();
    ResetPlayerForNewGame();
    InitializeGhosts();
    modeController_.Reset();
    gameState_ = GameState::Paused;
}

void Pacman::GameEngine::InitializePlayer()
{
    playerState_.Position = { GameConfig::PlayerStartX, GameConfig::PlayerStartY };
    playerState_.CurrentDirection = Direction::Left;
    playerState_.IsPoweredUp = false;
    desiredDirection_ = Direction::Left;
}

void Pacman::GameEngine::ResetPlayerForNewGame()
{
    InitializePlayer();
    playerState_.Score = 0;
    playerState_.Lives = GameConfig::StartingLives;
}

void Pacman::GameEngine::InitializeGhosts()
{
    ghostStates_[0] = GhostState{
    {13, 11}, Direction::Left, false, false,
    GhostType::Red, GhostMode::Scatter, "Red",
    {0, 0}, ghostAIs_[0]->GetScatterTarget()
    };
    ghostStates_[1] = GhostState{
        {13, 14}, Direction::Down, false, false,
        GhostType::Pink, GhostMode::Scatter, "Pink",
        {0, 0}, ghostAIs_[1]->GetScatterTarget()
    };
    ghostStates_[2] = GhostState{
        {11, 14}, Direction::Up, false, false,
        GhostType::Blue, GhostMode::Scatter, "Blue",
        {0, 0}, ghostAIs_[2]->GetScatterTarget()
    };
    ghostStates_[3] = GhostState{
        {15, 14}, Direction::Up, false, false,
        GhostType::Orange, GhostMode::Scatter, "Orange",
        {0, 0}, ghostAIs_[3]->GetScatterTarget()
    };
}

Pacman::Vector2 Pacman::GameEngine::GetNextPosition(const Vector2& from, Direction dir) const
{
    Vector2 delta = GetDirectionDelta(dir);
    Vector2 next = { from.X + delta.X, from.Y + delta.Y };
    return map_.WrapPosition(next);
}

bool Pacman::GameEngine::CanWalk(const Vector2& from, Direction dir) const
{
    return map_.IsWalkable(GetNextPosition(from, dir));
}

bool Pacman::GameEngine::CanGhostWalk(const Vector2& from, Direction dir, bool canUseGhostDoor) const
{
    return map_.IsGhostWalkable(GetNextPosition(from, dir), canUseGhostDoor);
}

float Pacman::GameEngine::GetCurrentGhostInterval() const
{
    if (modeController_.GetCurrentMode() == GhostMode::Frightened) {
        return GameConfig::GhostFrightenedStepInterval;
    }
    int pellets = map_.GetPelletCount();
    if (pellets <= GameConfig::ElroyDotsThreshold2) {
        return GameConfig::GhostStepInterval * 0.8f;
    }
    else if (pellets <= GameConfig::ElroyDotsThreshold1) {
        return GameConfig::GhostStepInterval * 0.9f;
    }
    return GameConfig::GhostStepInterval;
}

void Pacman::GameEngine::UpdatePlayer()
{
    if (desiredDirection_ != Direction::None && CanWalk(playerState_.Position, desiredDirection_)) {
        playerState_.CurrentDirection = desiredDirection_;
    }

    if (playerState_.CurrentDirection != Direction::None &&
        CanWalk(playerState_.Position, playerState_.CurrentDirection)) {
        playerState_.Position = GetNextPosition(playerState_.Position, playerState_.CurrentDirection);
        TryConsumeTile(playerState_.Position);
        NotifyPlayerState();
    }
}

bool Pacman::GameEngine::TryConsumeTile(const Vector2& pos)
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
        modeController_.TriggerFrightenedMode(GameConfig::PowerUpDuration);
        playerState_.IsPoweredUp = true;
        ghostsEatenThisPowerUp_ = 0;

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

void Pacman::GameEngine::UpdateGhosts()
{
    Vector2 blinkyPos = ghostStates_[0].Position;

    for (size_t i = 0; i < ghostStates_.size(); ++i) 
    {
        GhostState& ghost = ghostStates_[i];

        if (ghost.IsEaten) {
            UpdateEatenGhost(ghost);
            continue;
        }

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

void Pacman::GameEngine::CalculateGhostTarget(GhostState& ghost, size_t index, const Vector2& blinkyPos)
{
    GhostMode globalMode = modeController_.GetCurrentMode();

    if (ghost.IsFrightened) {
        ghost.TargetTile = ghost.Position;
        ghost.Mode = GhostMode::Frightened;
        return;
    }

    if (globalMode == GhostMode::Scatter) {
        ghost.TargetTile = ghost.ScatterTarget;
        ghost.Mode = GhostMode::Scatter;
    }
    else {
        ghost.TargetTile = ghostAIs_[index]->CalculateChaseTarget(ghost, playerState_, blinkyPos);
        ghost.Mode = GhostMode::Chase;
    }
}

Pacman::Direction Pacman::GameEngine::ChooseGhostDirection(const GhostState& ghost)
{
    Direction opposite = GetOppositeDirection(ghost.CurrentDirection);
    std::array<Direction, 4> priorities = {
        Direction::Up, Direction::Left, Direction::Down, Direction::Right
    };

    Direction bestDir = Direction::None;
    int bestDistSq = INT_MAX;
    bool canUseGhostDoor = IsInOrNearGhostHouse(ghost.Position);

    for (Direction dir : priorities) {
        if (dir == opposite) continue;
        if (!CanGhostWalk(ghost.Position, dir, canUseGhostDoor)) continue;

        if (ghost.IsFrightened) {
            // Random direction when frightened
            std::uniform_int_distribution<int> dist(0, 3);
            if (dist(rng_) == 0 || bestDir == Direction::None) {
                bestDir = dir;
            }
        }
        else {
            // Minimize distance to target
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

void Pacman::GameEngine::UpdateEatenGhost(GhostState& ghost)
{
    Vector2 houseTarget = { GameConfig::GhostHouseX, GameConfig::GhostHouseDoorY };

    if (ghost.Position == houseTarget ||
        (ghost.Position.Y >= GameConfig::GhostHouseDoorY &&
            std::abs(ghost.Position.X - GameConfig::GhostHouseX) <= 2)) {
        ghost.IsEaten = false;
        ghost.IsFrightened = modeController_.IsFrightened();
        ghost.Position = { GameConfig::GhostHouseX, GameConfig::GhostHouseY };
        return;
    }

    Direction opposite = GetOppositeDirection(ghost.CurrentDirection);
    std::array<Direction, 4> priorities = {
        Direction::Up, Direction::Left, Direction::Down, Direction::Right
    };

    Direction bestDir = Direction::None;
    int bestDistSq = INT_MAX;

    for (Direction dir : priorities) {
        if (dir == opposite && bestDir != Direction::None) continue;
        if (!CanGhostWalk(ghost.Position, dir, true)) continue;

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

void Pacman::GameEngine::ReverseGhostDirections()
{
    for (auto& ghost : ghostStates_) 
    {
        if (!ghost.IsEaten) 
        {
            ghost.CurrentDirection = GetOppositeDirection(ghost.CurrentDirection);
        }
    }
}

bool Pacman::GameEngine::IsInOrNearGhostHouse(const Vector2& pos) const
{
    return pos.Y >= 12 && pos.Y <= 16 && pos.X >= 10 && pos.X <= 17;
}

void Pacman::GameEngine::CheckCollisions()
{
    for (auto& ghost : ghostStates_) 
    {
        if (ghost.Position == playerState_.Position) 
        {
            if (ghost.IsEaten) 
                continue;

            if (ghost.IsFrightened) 
            {
                ghost.IsEaten = true;
                ghost.IsFrightened = false;

                // Award 50 points per eaten ghost
                playerState_.Score += GameConfig::GhostScore;
                ghostsEatenThisPowerUp_++;

                NotifyPlayerState();
                NotifyGhostsUpdated();
            }
            else 
            {
                HandlePlayerDeath();
                return;
            }
        }
    }
}

void Pacman::GameEngine::HandlePlayerDeath()
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
        InitializePlayer();
        InitializeGhosts();
        modeController_.Reset();
        playerStepTimer_ = 0.0f;
        ghostStepTimer_ = 0.0f;
        NotifyGhostsUpdated();
    }
}

void Pacman::GameEngine::NotifyAll()
{
    NotifyGameState();
    NotifyPlayerState();
    NotifyGhostsUpdated();
}

void Pacman::GameEngine::NotifyTileUpdated(const TileUpdate& update)
{
    for (auto& l : listeners_) if (l) l->OnTileUpdated(update);
}

void Pacman::GameEngine::NotifyPlayerState()
{
    playerState_.IsPoweredUp = modeController_.IsFrightened();
    for (auto& l : listeners_) if (l) l->OnPlayerStateChanged(playerState_);
}

void Pacman::GameEngine::NotifyGameState()
{
    for (auto& l : listeners_) if (l) l->OnGameStateChanged(gameState_);
}

void Pacman::GameEngine::NotifyGhostsUpdated()
{
    std::vector<GhostState> states(ghostStates_.begin(), ghostStates_.end());
    for (auto& l : listeners_) if (l) l->OnGhostsUpdated(states);
}

void Pacman::GameEngine::NotifyGhostModeChanged(GhostMode mode)
{
    for (auto& l : listeners_) if (l) l->OnGhostModeChanged(mode);
}


