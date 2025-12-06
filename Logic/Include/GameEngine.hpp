#pragma once
#include "IGameEngine.hpp"
#include "Map.hpp"
#include "IGhost.hpp"
#include "GameConfig.hpp"
#include "GhostModeController.hpp"
#include "GameTypes/GameState.hpp"
#include "GameTypes/TileType.hpp";

#include <array>
#include <mutex>
#include <random>

namespace Pacman
{
	class GameEngine : public IGameEngine
	{
    public:
        GameEngine();
        ~GameEngine() override = default;
        void StartNewGame() override;
        void Update(float deltaTime) override;
        void SetPaused(bool isPaused) override;
        void SetPlayerDirection(Direction direction) override;
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
        static std::shared_ptr<IGameEngine> CreateGameEngine();

    private:
        void InitializeGame();
        void InitializePlayer();
        void ResetPlayerForNewGame();
        void InitializeGhosts();
        Vector2 GetNextPosition(const Vector2& from, Direction dir) const;
        bool CanWalk(const Vector2& from, Direction dir) const;
        bool CanGhostWalk(const Vector2& from, Direction dir, bool canUseGhostDoor = false) const;
        float GetCurrentGhostInterval() const;
        void UpdatePlayer();
        bool TryConsumeTile(const Vector2& pos);
        void UpdateGhosts();
        void CalculateGhostTarget(GhostState& ghost, size_t index, const Vector2& blinkyPos);
        Direction ChooseGhostDirection(const GhostState& ghost);
        void UpdateEatenGhost(GhostState& ghost);
        void ReverseGhostDirections();
        bool IsInOrNearGhostHouse(const Vector2& pos) const;
        void CheckCollisions();
        void HandlePlayerDeath();
        void NotifyAll();
        void NotifyTileUpdated(const TileUpdate& update);
        void NotifyPlayerState();
        void NotifyGameState();
        void NotifyGhostsUpdated();
        void NotifyGhostModeChanged(GhostMode mode);

        mutable std::mt19937 rng_;
        mutable std::mutex mutex_;
        std::vector<std::shared_ptr<IEventListener>> listeners_;
        Map map_;
        GameState gameState_ = GameState::Paused;
        PlayerState playerState_{};
        Direction desiredDirection_ = Direction::None;
        std::array<GhostState, 4> ghostStates_;
        std::array<std::unique_ptr<IGhost>, 4> ghostAIs_;
        GhostModeController modeController_;
        int ghostsEatenThisPowerUp_ = 0;
        float playerStepTimer_ = 0.0f;
        float ghostStepTimer_ = 0.0f;
	};
}