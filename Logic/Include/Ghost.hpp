#pragma once
#include "IGhost.hpp"
#include "IGhostStrategy.hpp"
#include "GameTypes/GhostType.hpp"
#include <memory>

namespace Pacman
{
	class Ghost : public IGhost
	{
	public:
		Ghost(std::unique_ptr<IGhostStrategy> ghostStrategy, GhostType type);
		virtual Vector2 CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const override;
		virtual Vector2 GetScatterTarget() const override;
		virtual GhostType GetGhostType() const override;
	private:
		std::unique_ptr<IGhostStrategy> ghostStrategy_;
		GhostType type_;
	};
}