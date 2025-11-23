#pragma once

#include <cmath>
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
        GhostDoor
    };

    enum class GameState {
        Running,
        Paused,
        GameOver,
        Victory
    };

    enum class GhostType {
        Red,
        Pink,
        Blue,
        Orange
    };

    enum class GhostMode {
        Chase,
        Scatter,
        Frightened,
        Eaten
    };

    // Structures
    struct Vector2 {
        int X = 0;
        int Y = 0;

        bool operator==(const Vector2& other) const {
            return X == other.X && Y == other.Y;
        }

        bool operator!=(const Vector2& other) const {
            return !(*this == other);
        }

        Vector2 operator+(const Vector2& other) const {
            return {X + other.X, Y + other.Y};
        }

        Vector2 operator-(const Vector2& other) const {
            return {X - other.X, Y - other.Y};
        }

        Vector2 operator*(int scalar) const {
            return {X * scalar, Y * scalar};
        }

        int DistanceSquared(const Vector2& other) const {
            int dx = X - other.X;
            int dy = Y - other.Y;
            return dx * dx + dy * dy;
        }

        float Distance(const Vector2& other) const {
            return std::sqrt(static_cast<float>(DistanceSquared(other)));
        }
    };

    struct TileUpdate {
        Vector2 Position;
        TileType Type;
    };

    struct PlayerState {
        Vector2 Position{0, 0};
        Direction CurrentDirection = Direction::None;
        int Score = 0;
        bool IsPoweredUp = false;
        int Lives = 3;
    };

    struct GhostState {
        Vector2 Position{0, 0};
        Direction CurrentDirection = Direction::None;
        bool IsFrightened = false;
        bool IsEaten = false;
        GhostType Type = GhostType::Red;
        GhostMode Mode = GhostMode::Scatter;
        std::string Name;
        Vector2 TargetTile{0, 0};
        Vector2 ScatterTarget{0, 0};
    };

    // ==================== Helper Functions ====================

    inline Vector2 GetDirectionDelta(Direction dir) {
        switch (dir) {
        case Direction::Up:    return {0, -1};
        case Direction::Down:  return {0, 1};
        case Direction::Left:  return {-1, 0};
        case Direction::Right: return {1, 0};
        default:               return {0, 0};
        }
    }

    inline Direction GetOppositeDirection(Direction dir) {
        switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
        }
    }

}
