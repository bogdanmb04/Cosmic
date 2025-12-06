#pragma once

#include "GameConfig.hpp"
#include "GameTypes/GhostMode.hpp"

namespace Pacman 
{

    /// @brief Controls ghost mode timing (Chase/Scatter waves)
    /// 
    /// Original Pac-Man pattern (Level 1):
    /// Scatter(7s) -> Chase(20s) -> Scatter(7s) -> Chase(20s) ->
    /// Scatter(5s) -> Chase(20s) -> Scatter(5s) -> Chase(forever)
    class GhostModeController 
    {
    public:
        GhostModeController();
        void Reset();
        void Update(float deltaTime);
        void TriggerFrightenedMode(float duration = GameConfig::PowerUpDuration);
        bool ShouldReverseDirection();
        GhostMode GetCurrentMode() const;
        bool IsFrightened() const;
        float GetFrightenedTimeRemaining() const;
        bool IsFrightenedWarning() const;

    private:
        float GetCurrentPhaseDuration() const;
        int currentWave_ = 0;
        float waveTimer_ = 0.0f;
        bool isScatterPhase_ = true;
        bool isPermanentChase_ = false;
        bool modeJustChanged_ = false;
        float frightenedTimer_ = 0.0f;
        bool isFrightened_ = false;
    };

}