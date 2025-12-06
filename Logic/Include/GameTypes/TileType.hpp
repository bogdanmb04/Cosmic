#pragma once
#include <cstdint>

namespace Pacman
{
	enum class TileType : std::uint8_t
	{
        Wall,
        Path,
        Pellet,
        PowerPellet,
        Empty,
        GhostDoor
	};
}