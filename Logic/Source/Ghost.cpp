#include "IGhost.hpp"
#include "GameConfig.hpp"
#include "Ghost.hpp"

namespace Pacman 
{
    Ghost::Ghost(std::unique_ptr<IGhostStrategy> ghostStrategy, GhostType type)
        : ghostStrategy_{std::move(ghostStrategy)}
        , type_{type}
    {
        //EMPTY
    }

    Vector2 Ghost::CalculateChaseTarget(const GhostState& ghost, const PlayerState& player, const Vector2& blinkyPosition) const
    {
        return ghostStrategy_->CalculateChaseTarget(ghost, player, blinkyPosition);
    }

    Vector2 Ghost::GetScatterTarget() const
    {
        return ghostStrategy_->GetScatterTarget();
    }

    GhostType Ghost::GetGhostType() const
    {
        return type_;
    }
}