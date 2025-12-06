#pragma once
#include "Vector2.hpp"
#include "TileType.hpp"

namespace Pacman
{
    struct TileUpdate 
    {
        Vector2 Position;
        TileType Type;
    };
}