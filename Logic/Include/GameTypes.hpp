#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Pacman {

    // Enumerations
    enum class Direction {
        Up,
        Down,
        Left,
        Right,
        None
    };

    enum class TileType {
        Wall,
        Path,
        Pellet,
        PowerPellet,
        Empty,
        Fruit,
        GhostDoor
    };

    enum class GameState {
        Running,
        Paused,
        GameOver,
        Victory
    };

    enum class GhostType {
        Blinky,  // Red - Aggressive chaser
        Pinky,   // Pink - Ambusher
        Inky,    // Cyan - Unpredictable
        Clyde    // Orange - Random
    };

    // Structures
    struct Vector2 {
        int X;
        int Y;

        bool operator==(const Vector2& other) const {
            return X == other.X && Y == other.Y;
        }
    };

    struct TileUpdate {
        Vector2 Position;
        TileType Type;
    };

    struct PlayerState {
        Vector2 Position;
        Direction CurrentDirection;
        int Score;
        bool IsPoweredUp;
        int Lives;
    };

    struct GhostState {
        Vector2 Position;
        Direction CurrentDirection;
        bool IsFrightened;
        GhostType Type;
        std::string Name;
    };

} // namespace Pacman
