#pragma once

#include "GameTypes.hpp"
#include "GameConfig.hpp"

namespace Pacman {

    /// @brief Controls ghost mode timing (Chase/Scatter waves)
    /// 
    /// Original Pac-Man pattern (Level 1):
    /// Scatter(7s) -> Chase(20s) -> Scatter(7s) -> Chase(20s) ->
    /// Scatter(5s) -> Chase(20s) -> Scatter(5s) -> Chase(forever)
    class GhostModeController {
    public:
        GhostModeController() { Reset(); }

        void Reset() {
            currentWave_ = 0;
            waveTimer_ = 0.0f;
            isScatterPhase_ = true;
            frightenedTimer_ = 0.0f;
            isFrightened_ = false;
            isPermanentChase_ = false;
            modeJustChanged_ = false;
        }

        void Update(float deltaTime) {
            if (isFrightened_) {
                frightenedTimer_ -= deltaTime;
                if (frightenedTimer_ <= 0.0f) {
                    isFrightened_ = false;
                    frightenedTimer_ = 0.0f;
                }
                return; // Timer paused during frightened
            }

            if (isPermanentChase_) return;

            waveTimer_ += deltaTime;
            float phaseDuration = GetCurrentPhaseDuration();

            if (waveTimer_ >= phaseDuration) {
                waveTimer_ -= phaseDuration;

                if (isScatterPhase_) {
                    isScatterPhase_ = false;
                } else {
                    currentWave_++;
                    if (currentWave_ >= GameConfig::ScatterChaseWaves) {
                        isPermanentChase_ = true;
                        isScatterPhase_ = false;
                    } else {
                        isScatterPhase_ = true;
                    }
                }
                modeJustChanged_ = true;
            }
        }

        void TriggerFrightenedMode(float duration = GameConfig::PowerUpDuration) {
            isFrightened_ = true;
            frightenedTimer_ = duration;
        }

        bool ShouldReverseDirection() {
            bool result = modeJustChanged_;
            modeJustChanged_ = false;
            return result;
        }

        GhostMode GetCurrentMode() const {
            if (isFrightened_) return GhostMode::Frightened;
            return isScatterPhase_ ? GhostMode::Scatter : GhostMode::Chase;
        }

        bool IsFrightened() const { return isFrightened_; }
        float GetFrightenedTimeRemaining() const { return frightenedTimer_; }
        
        bool IsFrightenedWarning() const {
            return isFrightened_ && frightenedTimer_ <= GameConfig::PowerUpWarningTime;
        }

    private:
        float GetCurrentPhaseDuration() const {
            if (currentWave_ >= GameConfig::ScatterChaseWaves) return 99999.0f;
            return isScatterPhase_ 
                ? GameConfig::ScatterDurations[currentWave_]
                : GameConfig::ChaseDurations[currentWave_];
        }

        int currentWave_ = 0;
        float waveTimer_ = 0.0f;
        bool isScatterPhase_ = true;
        bool isPermanentChase_ = false;
        bool modeJustChanged_ = false;
        float frightenedTimer_ = 0.0f;
        bool isFrightened_ = false;
    };

}