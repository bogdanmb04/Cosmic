/**
 * @file GhostModeController.cpp
 * @brief Implementation of the ghost behavior state machine.
 * @details Manages the timing for Scatter/Chase waves and the Frightened mode interrupt.
 */

#include "GhostModeController.hpp"

namespace Pacman
{
    /**
     * @brief Constructor. Initializes the controller in the reset state.
     */
    GhostModeController::GhostModeController()
    {
        Reset();
    }

    /**
     * @brief Resets the controller to the beginning of the level (Wave 0, Scatter Phase).
     */
    void GhostModeController::Reset()
    {
        currentWave_ = 0;
        waveTimer_ = 0.0f;
        isScatterPhase_ = true; // Ghosts always start in Scatter mode
        frightenedTimer_ = 0.0f;
        isFrightened_ = false;
        isPermanentChase_ = false;
        modeJustChanged_ = false;
    }

    /**
     * @brief Updates the internal timers and switches modes when necessary.
     * @details The logic follows standard Pac-Man rules:
     * 1. If Frightened, decrement that timer only (Wave timer is paused).
     * 2. If in Permanent Chase (end of level logic), do nothing.
     * 3. Otherwise, increment Wave timer.
     * 4. If Wave timer exceeds duration, switch mode (Scatter <-> Chase) and advance wave count.
     * * @param deltaTime Time elapsed since the last frame (in seconds).
     */
    void GhostModeController::Update(float deltaTime)
    {
        // 1. Handle Frightened Mode (Priority)
        if (isFrightened_) {
            frightenedTimer_ -= deltaTime;
            if (frightenedTimer_ <= 0.0f) {
                isFrightened_ = false;
                frightenedTimer_ = 0.0f;
            }
            return; // IMPORTANT: The Scatter/Chase timer pauses while Frightened!
        }

        // 2. Handle Permanent Chase (End game state)
        if (isPermanentChase_) return;

        // 3. Normal Scatter/Chase Logic
        waveTimer_ += deltaTime;
        float phaseDuration = GetCurrentPhaseDuration();

        if (waveTimer_ >= phaseDuration) {
            waveTimer_ -= phaseDuration;

            // Toggle phase
            if (isScatterPhase_) {
                isScatterPhase_ = false; // Switch to Chase
            }
            else {
                // End of a full Scatter/Chase cycle
                currentWave_++;
                if (currentWave_ >= GameConfig::ScatterChaseWaves) {
                    isPermanentChase_ = true;
                    isScatterPhase_ = false;
                }
                else {
                    isScatterPhase_ = true; // Back to Scatter
                }
            }
            // Signal that ghosts should reverse direction
            modeJustChanged_ = true;
        }
    }

    /**
     * @brief Activates Frightened mode when a Power Pellet is eaten.
     * @param duration How long the mode lasts (in seconds).
     */
    void Pacman::GhostModeController::TriggerFrightenedMode(float duration)
    {
        isFrightened_ = true;
        frightenedTimer_ = duration;
    }

    /**
     * @brief Checks if a mode switch occurred in the last frame.
     * @details Ghosts in Pac-Man are required to reverse direction immediately
     * when switching between Scatter and Chase modes.
     * @return true if direction should be reversed, false otherwise.
     */
    bool GhostModeController::ShouldReverseDirection()
    {
        bool result = modeJustChanged_;
        modeJustChanged_ = false; // Reset flag after reading
        return result;
    }

    /**
     * @brief Determines the current effective behavior mode.
     * @return The active mode (Frightened overrides everything, otherwise Scatter or Chase).
     */
    GhostMode GhostModeController::GetCurrentMode() const
    {
        if (isFrightened_) return GhostMode::Frightened;
        return isScatterPhase_ ? GhostMode::Scatter : GhostMode::Chase;
    }

    /**
     * @brief Checks if ghosts are currently vulnerable.
     */
    bool GhostModeController::IsFrightened() const
    {
        return isFrightened_;
    }

    /**
     * @brief Returns time remaining for the power-up effect.
     */
    float GhostModeController::GetFrightenedTimeRemaining() const
    {
        return frightenedTimer_;
    }

    /**
     * @brief Checks if the power-up is about to expire.
     * @details Used by the renderer to make ghosts flash white.
     */
    bool GhostModeController::IsFrightenedWarning() const
    {
        return isFrightened_ && frightenedTimer_ <= GameConfig::PowerUpWarningTime;
    }

    /**
     * @brief Looks up the duration for the current wave index.
     * @return Duration in seconds.
     */
    float GhostModeController::GetCurrentPhaseDuration() const
    {
        if (currentWave_ >= GameConfig::ScatterChaseWaves) return 99999.0f; // Infinite

        return isScatterPhase_
            ? GameConfig::ScatterDurations[currentWave_]
            : GameConfig::ChaseDurations[currentWave_];
    }
}