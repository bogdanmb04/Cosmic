#include "GhostStrategies/BlueStrategy.hpp"
#include "GameConfig.hpp"

namespace Pacman
{
	Vector2 BlueStrategy::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
	{
        Vector2 pivotPoint = player.Position;
        Vector2 delta = GetDirectionDelta(player.CurrentDirection);

        pivotPoint.X += delta.X * GameConfig::BlueTargetAhead;
        pivotPoint.Y += delta.Y * GameConfig::BlueTargetAhead;

        if (player.CurrentDirection == Direction::Up) {
            pivotPoint.X -= GameConfig::BlueTargetAhead;
        }

        Vector2 target;
        target.X = 2 * pivotPoint.X - blinkyPosition.X;
        target.Y = 2 * pivotPoint.Y - blinkyPosition.Y;

        return target;
	}

	Vector2 BlueStrategy::GetScatterTarget() const
	{
        return Vector2{ GameConfig::BlueScatterX, GameConfig::BlueScatterY };
	}
}
