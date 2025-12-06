#pragma once
#include "Direction.hpp"

namespace Pacman
{
    struct Vector2 
    {
        int X = 0;
        int Y = 0;

        bool operator==(const Vector2& other) const;

        bool operator!=(const Vector2& other) const;

        Vector2 operator+(const Vector2& other) const;

        Vector2 operator-(const Vector2& other) const;

        Vector2 operator*(int scalar) const;

        int DistanceSquared(const Vector2& other) const;

        float Distance(const Vector2& other) const;

        bool operator<(const Vector2& other) const;
    };

    inline Vector2 GetDirectionDelta(Direction dir) 
    {
        switch (dir) 
        {
            case Direction::Up:    return { 0, -1 };
            case Direction::Down:  return { 0, 1 };
            case Direction::Left:  return { -1, 0 };
            case Direction::Right: return { 1, 0 };
            default:               return { 0, 0 };
        }
    }
}