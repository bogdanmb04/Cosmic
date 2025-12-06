#pragma once

#include "IEventListener.hpp"
#include <memory>
#include <vector>

namespace Pacman {

    class IGameEngine {
    public:
        virtual ~IGameEngine() = default;

        virtual void StartNewGame() = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void SetPaused(bool isPaused) = 0;
        virtual void SetPlayerDirection(Direction direction) = 0;

        virtual GameState GetState() const = 0;
        virtual PlayerState GetPlayerState() const = 0;
        virtual Vector2 GetMapSize() const = 0;
        virtual TileType GetTileAt(const Vector2& position) const = 0;
        virtual std::vector<Vector2> GetPelletPositions() const = 0;
        virtual int GetPelletCount() const = 0;
        virtual std::vector<GhostState> GetGhostStates() const = 0;
        virtual GhostMode GetGlobalGhostMode() const = 0;

        virtual void AddListener(std::shared_ptr<IEventListener> listener) = 0;
        virtual void RemoveListener(std::shared_ptr<IEventListener> listener) = 0;
    };

    std::shared_ptr<IGameEngine> CreateGameEngine();

}