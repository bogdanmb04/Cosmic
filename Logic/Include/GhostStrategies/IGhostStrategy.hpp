#pragma once
#include "GameTypes/Vector2.hpp"
#include "GameTypes/PlayerState.hpp"
#include "GameTypes/GhostState.hpp"

namespace Pacman
{
	class IGhostStrategy
	{
	public:
		virtual ~IGhostStrategy() = default;
		virtual Vector2 CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const = 0;
		virtual Vector2 GetScatterTarget() const = 0;
	};
}