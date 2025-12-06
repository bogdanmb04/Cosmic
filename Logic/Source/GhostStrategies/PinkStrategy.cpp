#include "GhostStrategies/PinkStrategy.hpp"
#include "GameConfig.hpp"

namespace Pacman
{
	Vector2 PinkStrategy::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
	{
		Vector2 target = player.Position;
		Vector2 delta = GetDirectionDelta(player.CurrentDirection);

		target.X += delta.X * GameConfig::PinkTargetAhead;
		target.Y += delta.Y * GameConfig::PinkTargetAhead;

		if (player.CurrentDirection == Direction::Up) {
			target.X -= GameConfig::PinkTargetAhead;
		}

		return target;
	}

	Vector2 PinkStrategy::GetScatterTarget() const
	{
		return { GameConfig::PinkScatterX, GameConfig::PinkScatterY };
	}
}
