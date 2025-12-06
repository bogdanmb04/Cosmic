#pragma once
#include "IGhostStrategy.hpp"

namespace Pacman
{
	class PinkStrategy : public IGhostStrategy
	{
		virtual Vector2 CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const override;
		virtual Vector2 GetScatterTarget() const override;
	};
}