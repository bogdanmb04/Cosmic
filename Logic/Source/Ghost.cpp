#include "IGhost.hpp"
#include "GameConfig.hpp"

namespace Pacman {
    
    class RedAI : public IGhost {
    public:
        Vector2 CalculateChaseTarget(
            const GhostState&,
            const PlayerState& player,
            const Vector2&) const override {
            return player.Position;
        }

        Vector2 GetScatterTarget() const override {
            return {GameConfig::RedScatterX, GameConfig::RedScatterY};
        }

        GhostType GetGhostType() const override {
            return GhostType::Red;
        }
    };

    class PinkAI : public IGhost {
    public:
        Vector2 CalculateChaseTarget(
            const GhostState&,
            const PlayerState& player,
            const Vector2&) const override {
            Vector2 target = player.Position;
            Vector2 delta = GetDirectionDelta(player.CurrentDirection);

            target.X += delta.X * GameConfig::PinkTargetAhead;
            target.Y += delta.Y * GameConfig::PinkTargetAhead;

            if (player.CurrentDirection == Direction::Up) {
                target.X -= GameConfig::PinkTargetAhead;
            }
            
            return target;
        }

        Vector2 GetScatterTarget() const override {
            return {GameConfig::PinkScatterX, GameConfig::PinkScatterY};
        }

        GhostType GetGhostType() const override {
            return GhostType::Pink;
        }
    };

    class BlueAI : public IGhost {
    public:
        Vector2 CalculateChaseTarget(
            const GhostState&,
            const PlayerState& player,
            const Vector2& redPosition) const override {
            Vector2 pivotPoint = player.Position;
            Vector2 delta = GetDirectionDelta(player.CurrentDirection);
            
            pivotPoint.X += delta.X * GameConfig::BlueTargetAhead;
            pivotPoint.Y += delta.Y * GameConfig::BlueTargetAhead;

            if (player.CurrentDirection == Direction::Up) {
                pivotPoint.X -= GameConfig::BlueTargetAhead;
            }

            Vector2 target;
            target.X = 2 * pivotPoint.X - redPosition.X;
            target.Y = 2 * pivotPoint.Y - redPosition.Y;
            
            return target;
        }

        Vector2 GetScatterTarget() const override {
            return {GameConfig::BlueScatterX, GameConfig::BlueScatterY};
        }

        GhostType GetGhostType() const override {
            return GhostType::Blue;
        }
    };

    class OrangeAI : public IGhost {
    public:
        Vector2 CalculateChaseTarget(
            const GhostState& ghost,
            const PlayerState& player,
            const Vector2&) const override
        {
            float distance = ghost.Position.Distance(player.Position);

            if (distance > static_cast<float>(GameConfig::OrangeShyDistance)) {
                return player.Position;
            }

            return GetScatterTarget();
        }

        Vector2 GetScatterTarget() const override {
            return {GameConfig::OrangeScatterX, GameConfig::OrangeScatterY};
        }

        GhostType GetGhostType() const override {
            return GhostType::Orange;
        }
    };

    // Factory Functions

    std::unique_ptr<IGhost> CreateRedAI() {
        return std::make_unique<RedAI>();
    }

    std::unique_ptr<IGhost> CreatePinkAI() {
        return std::make_unique<PinkAI>();
    }

    std::unique_ptr<IGhost> CreateBlueAI() {
        return std::make_unique<BlueAI>();
    }

    std::unique_ptr<IGhost> CreateOrangeAI() {
        return std::make_unique<OrangeAI>();
    }

    std::unique_ptr<IGhost> CreateGhostAI(GhostType type) {
        switch (type) {
            case GhostType::Red: return CreateRedAI();
            case GhostType::Pink:  return CreatePinkAI();
            case GhostType::Blue:   return CreateBlueAI();
            case GhostType::Orange:  return CreateOrangeAI();
            default:                return CreateRedAI();
        }
    }

}