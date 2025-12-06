#pragma once
#include "Vector2.hpp"
#include "Direction.hpp"

namespace Pacman
{
    struct PlayerState 
    {
        Vector2 Position{ 0, 0 };
        Direction CurrentDirection = Direction::None;
        int Score = 0;
        bool IsPoweredUp = false;
        int Lives = 3;
    };
}