#pragma once
#include <cstdint>

namespace Pacman 
{
	enum class Direction : std::uint8_t
	{
		Up,
		Down,
		Left,
		Right,
		None
	};

	inline Direction GetOppositeDirection(Direction dir)
	{
		switch (dir) 
		{
			case Direction::Up:    return Direction::Down;
			case Direction::Down:  return Direction::Up;
			case Direction::Left:  return Direction::Right;
			case Direction::Right: return Direction::Left;
			default:               return Direction::None;
		}
	}
}