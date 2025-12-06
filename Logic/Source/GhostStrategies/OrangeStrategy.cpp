#include "GhostStrategies/OrangeStrategy.hpp"
#include "GameConfig.hpp"

namespace Pacman
{
	Vector2 OrangeStrategy::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
	{
		float distance = ghost.Position.Distance(player.Position);

		if (distance > static_cast<float>(GameConfig::OrangeShyDistance)) {
			return player.Position;
		}

		return GetScatterTarget();
	}

	Vector2 OrangeStrategy::GetScatterTarget() const
	{
		return { GameConfig::OrangeScatterX, GameConfig::OrangeScatterY };
	}
}
