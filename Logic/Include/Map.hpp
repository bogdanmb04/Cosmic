#pragma once

#include <vector>
#include <string>
#include <array>
#include "GameTypes/TileType.hpp"
#include "GameTypes/Vector2.hpp"

namespace Pacman 
{
    class Map 
    {
    public:
        Map();
        void Initialize();
        TileType GetTileAt(const Vector2& pos) const;
        void SetTileAt(const Vector2& pos, TileType type);
        bool IsInBounds(const Vector2& pos) const;
        bool IsWalkable(const Vector2& pos) const;
        bool IsGhostWalkable(const Vector2& pos, bool canUseGhostDoor = false) const;
        Vector2 WrapPosition(const Vector2& pos) const;
        int GetWidth() const;
        int GetHeight() const;
        Vector2 GetSize() const;
        int GetPelletCount() const;
        int GetInitialPelletCount() const;
        std::vector<Vector2> GetPelletPositions() const;

    private:
        int width_ = 0;
        int height_ = 0;
        std::vector<TileType> tiles_;
        int pelletCount_ = 0;
        int initialPelletCount_ = 0;
    };
}