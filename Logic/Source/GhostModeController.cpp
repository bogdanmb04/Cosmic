#include "GhostModeController.hpp"

namespace Pacman
{
    GhostModeController::GhostModeController()
    {
        Reset();
    }

    void GhostModeController::Reset()
	{
        currentWave_ = 0;
        waveTimer_ = 0.0f;
        isScatterPhase_ = true;
        frightenedTimer_ = 0.0f;
        isFrightened_ = false;
        isPermanentChase_ = false;
        modeJustChanged_ = false;
	}

    void GhostModeController::Update(float deltaTime)
    {
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
            }
            else {
                currentWave_++;
                if (currentWave_ >= GameConfig::ScatterChaseWaves) {
                    isPermanentChase_ = true;
                    isScatterPhase_ = false;
                }
                else {
                    isScatterPhase_ = true;
                }
            }
            modeJustChanged_ = true;
        }
    }

    void Pacman::GhostModeController::TriggerFrightenedMode(float duration)
    {
        isFrightened_ = true;
        frightenedTimer_ = duration;
    }

    bool GhostModeController::ShouldReverseDirection()
    {
        bool result = modeJustChanged_;
        modeJustChanged_ = false;
        return result;
    }

    GhostMode GhostModeController::GetCurrentMode() const
    {
        if (isFrightened_) return GhostMode::Frightened;
        return isScatterPhase_ ? GhostMode::Scatter : GhostMode::Chase;
    }

    bool GhostModeController::IsFrightened() const
    {
        return isFrightened_;
    }

    float GhostModeController::GetFrightenedTimeRemaining() const
    {
        return frightenedTimer_;
    }

    bool GhostModeController::IsFrightenedWarning() const
    {
        return isFrightened_ && frightenedTimer_ <= GameConfig::PowerUpWarningTime;
    }

    float GhostModeController::GetCurrentPhaseDuration() const
    {
        if (currentWave_ >= GameConfig::ScatterChaseWaves) return 99999.0f;
        return isScatterPhase_
            ? GameConfig::ScatterDurations[currentWave_]
            : GameConfig::ChaseDurations[currentWave_];
    }

    /*

    */
}
