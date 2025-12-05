/**
 * @file GameTypes.hpp
 * @brief Defines fundamental data types, enumerations, and structures shared throughout the project.
 * @details Contains graphic primitives (Vector2), game states, and entity definitions (Player/Ghost).
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace Pacman {

    /**
     * @enum Direction
     * @brief Possible movement directions within the grid.
     */
    enum class Direction {
        Up,     ///< Move up (Y decreases).
        Down,   ///< Move down (Y increases).
        Left,   ///< Move left (X decreases).
        Right,  ///< Move right (X increases).
        None    ///< Stationary (no direction).
    };

    /**
     * @enum TileType
     * @brief Types of elements that can compose the map (maze).
     */
    enum class TileType {
        Wall,        ///< Solid wall, blocks movement.
        Path,        ///< Clear path that can be walked on.
        Pellet,      ///< Small dot that provides score (10 points).
        PowerPellet, ///< Large dot that makes ghosts vulnerable (50 points).
        Empty,       ///< Empty space outside the playable map.
        GhostDoor    ///< Gate to the ghost house (only ghosts can pass).
    };

    /**
     * @enum GameState
     * @brief General states of the application.
     */
    enum class GameState {
        Running,    ///< Game is active and updating.
        Paused,     ///< Game is temporarily paused.
        GameOver,   ///< Player has lost all lives.
        Victory     ///< All pellets have been eaten (level complete).
    };

    /**
     * @enum GhostType
     * @brief Identifies each individual ghost (for color and behavior).
     */
    enum class GhostType {
        Red,    ///< Blinky (Shadow) - Directly chases the player.
        Pink,   ///< Pinky (Speedy) - Tries to anticipate movement (ambush).
        Blue,   ///< Inky (Bashful) - Complex behavior based on Blinky.
        Orange  ///< Clyde (Pokey) - Chases, but flees if too close.
    };

    /**
     * @enum GhostMode
     * @brief Current state of the ghost AI.
     */
    enum class GhostMode {
        Scatter,    ///< Ghost retreats to its safety corner.
        Chase,      ///< Ghost actively chases the player.
        Frightened, ///< Ghost is blue and runs from player (after PowerPellet).
        Eaten       ///< Ghost was eaten and returns to base as eyes.
    };

    /**
     * @struct Vector2
     * @brief Utility for 2D grid coordinates.
     * @details Includes math operators to simplify distance and movement calculations.
     */
    struct Vector2 {
        int X = 0; ///< Position on the horizontal axis (Column).
        int Y = 0; ///< Position on the vertical axis (Row).

        /// @brief Checks if two vectors are identical.
        bool operator==(const Vector2& other) const {
            return X == other.X && Y == other.Y;
        }

        /// @brief Checks if two vectors are different.
        bool operator!=(const Vector2& other) const {
            return !(*this == other);
        }

        /// @brief Adds two vectors (e.g., Position + Direction).
        Vector2 operator+(const Vector2& other) const {
            return { X + other.X, Y + other.Y };
        }

        /// @brief Subtracts two vectors (e.g., Destination - Source).
        Vector2 operator-(const Vector2& other) const {
            return { X - other.X, Y - other.Y };
        }

        /// @brief Multiplies vector by a scalar (for scaling).
        Vector2 operator*(int scalar) const {
            return { X * scalar, Y * scalar };
        }

        /**
         * @brief Calculates squared distance to another point.
         * @details Faster than Distance() because it avoids the square root (std::sqrt).
         * Useful for comparing distances (e.g., which path is shorter?).
         */
        int DistanceSquared(const Vector2& other) const {
            int dx = X - other.X;
            int dy = Y - other.Y;
            return dx * dx + dy * dy;
        }

        /// @brief Calculates actual Euclidean distance (uses square root).
        float Distance(const Vector2& other) const {
            return std::sqrt(static_cast<float>(DistanceSquared(other)));
        }

        /**
         * @brief Ordering operator (needed for std::map or std::set).
         * @details Orders first by X, then by Y.
         */
        bool operator<(const Vector2& other) const {
            if (X < other.X) return true;
            if (X > other.X) return false;
            return Y < other.Y;
        }
    };

    /**
     * @struct TileUpdate
     * @brief Structure used to notify a single cell change.
     * @details Optimization: avoids redrawing the entire map.
     */
    struct TileUpdate {
        Vector2 Position; ///< Coordinate of the modified cell.
        TileType Type;    ///< New cell type (e.g., Path after a Pellet is eaten).
    };

    /**
     * @struct PlayerState
     * @brief Complete state of the player (Pacman).
     */
    struct PlayerState {
        Vector2 Position{ 0, 0 };                     ///< Current grid position.
        Direction CurrentDirection = Direction::None; ///< Direction currently facing.
        int Score = 0;                                ///< Current score.
        bool IsPoweredUp = false;                     ///< True if PowerPellet effect is active.
        int Lives = 3;                                ///< Lives remaining.
    };

    /**
     * @struct GhostState
     * @brief Complete state of a ghost.
     */
    struct GhostState {
        Vector2 Position{ 0, 0 };                     ///< Current position.
        Direction CurrentDirection = Direction::None; ///< Movement direction.
        bool IsFrightened = false;                    ///< If it can be eaten.
        bool IsEaten = false;                         ///< If it is in "eyes" mode returning home.
        GhostType Type = GhostType::Red;              ///< Ghost identity.
        GhostMode Mode = GhostMode::Scatter;          ///< Current behavior.
        std::string Name;                             ///< Name (e.g., "Blinky").
        Vector2 TargetTile{ 0, 0 };                   ///< Current AI target (for debug).
        Vector2 ScatterTarget{ 0, 0 };                ///< Specific safety corner for this ghost.
    };

    /**
     * @brief Converts a Direction (Enum) into a movement vector (Delta).
     * @param dir The desired direction.
     * @return Vector2 (-1, 0) for left, (0, 1) for down, etc.
     */
    inline Vector2 GetDirectionDelta(Direction dir) {
        switch (dir) {
        case Direction::Up:    return { 0, -1 };
        case Direction::Down:  return { 0, 1 };
        case Direction::Left:  return { -1, 0 };
        case Direction::Right: return { 1, 0 };
        default:               return { 0, 0 };
        }
    }

    /**
     * @brief Returns the opposite direction of the given one.
     * @details Useful for AI (ghosts cannot turn 180 degrees except in special cases).
     */
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