#include "GhostStrategies/RedStrategy.hpp"
#include "GameConfig.hpp"

namespace Pacman
{
	Vector2 RedStrategy::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
	{
		return player.Position;
	}

	Vector2 RedStrategy::GetScatterTarget() const
	{
		return { GameConfig::RedScatterX, GameConfig::RedScatterY };
	}
}
