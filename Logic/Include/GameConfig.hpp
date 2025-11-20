#pragma once

namespace Pacman {

    /// @brief Game configuration constants
    struct GameConfig {
        static constexpr int MapWidth = 28;
        static constexpr int MapHeight = 31;

        // Scoring
        static constexpr int PelletScore = 10;
        static constexpr int PowerPelletScore = 50;
        static constexpr int GhostScore = 200;

        // Timing (in seconds)
        static constexpr float PlayerStepInterval = 0.12f;
        static constexpr float GhostStepInterval = 0.16f;
        static constexpr float PowerUpDuration = 4.0f;

        // Gameplay
        static constexpr int StartingLives = 3;
        static constexpr int MaxLives = 5;

        // Animation
        static constexpr int TileSize = 16;
        static constexpr int PacmanFrameCount = 4;
        static constexpr int GhostFrameCount = 2;
    };

}
