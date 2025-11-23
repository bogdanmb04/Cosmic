#pragma once

namespace Pacman {

    struct GameConfig {
        // Map
        static constexpr int MapWidth = 28;
        static constexpr int MapHeight = 31;

        // Scoring
        static constexpr int PelletScore = 10;
        static constexpr int PowerPelletScore = 50;
        static constexpr int GhostScore = 200;
        static constexpr int GhostScoreMultiplier = 2;

        // Movement timing (seconds)
        static constexpr float PlayerStepInterval = 0.12f;
        static constexpr float GhostStepInterval = 0.16f;
        static constexpr float GhostFrightenedStepInterval = 0.24f;
        static constexpr float GhostEatenStepInterval = 0.06f;

        // Power-up
        static constexpr float PowerUpDuration = 6.0f;
        static constexpr float PowerUpWarningTime = 2.0f;

        // Ghost mode timing (Level 1)
        static constexpr int ScatterChaseWaves = 4;
        static constexpr float ScatterDurations[] = {7.0f, 7.0f, 5.0f, 5.0f};
        static constexpr float ChaseDurations[] = {20.0f, 20.0f, 20.0f, 99999.0f};

        // Ghost targeting constants
        static constexpr int PinkTargetAhead = 4;
        static constexpr int BlueTargetAhead = 2;
        static constexpr int OrangeShyDistance = 8;
        static constexpr int ElroyDotsThreshold1 = 20;
        static constexpr int ElroyDotsThreshold2 = 10;

        // Scatter corners (outside maze)
        static constexpr int RedScatterX = 25;
        static constexpr int RedScatterY = -3;
        static constexpr int PinkScatterX = 2;
        static constexpr int PinkScatterY = -3;
        static constexpr int BlueScatterX = 27;
        static constexpr int BlueScatterY = 34;
        static constexpr int OrangeScatterX = 0;
        static constexpr int OrangeScatterY = 34;

        // Ghost house
        static constexpr int GhostHouseX = 13;
        static constexpr int GhostHouseY = 14;
        static constexpr int GhostHouseDoorY = 12;

        // Player start
        static constexpr int PlayerStartX = 13;
        static constexpr int PlayerStartY = 23;

        // Gameplay
        static constexpr int StartingLives = 3;
        static constexpr int MaxLives = 5;
        static constexpr int ExtraLifeScore = 10000;

        // Animation
        static constexpr int TileSize = 16;
        static constexpr int PacmanFrameCount = 4;
        static constexpr int GhostFrameCount = 2;
    };

}