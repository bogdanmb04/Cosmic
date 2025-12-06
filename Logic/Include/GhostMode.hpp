#pragma once
#include <cstdint>

namespace Pacman
{
    enum class GhostMode : std::uint8_t
    {
        Scatter,
        Chase,
        Frightened,
        Eaten
    };
}