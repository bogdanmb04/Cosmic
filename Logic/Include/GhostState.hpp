#pragma once
#include "Vector2.hpp"
#include "Direction.hpp"
#include "GhostType.hpp"
#include "GhostMode.hpp"
#include <string>

namespace Pacman
{
    struct GhostState 
    {
        Vector2 Position{ 0, 0 };
        Direction CurrentDirection = Direction::None;
        bool IsFrightened = false;
        bool IsEaten = false;
        GhostType Type = GhostType::Red;
        GhostMode Mode = GhostMode::Scatter;
        std::string Name;
        Vector2 TargetTile{ 0, 0 };
        Vector2 ScatterTarget{ 0, 0 };
    };
}