#pragma once
#include <cstdint>

namespace Pacman
{
    enum class GameState : std::uint8_t
    {
        Running,
        Paused,
        GameOver,
        Victory
    };
}