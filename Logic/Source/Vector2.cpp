#include "Vector2.hpp"
#include <cmath>

namespace Pacman
{
	bool Vector2::operator==(const Vector2& other) const
	{
		return X == other.X && Y == other.Y;
	}

	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}
	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2{ X + other.X, Y + other.Y };
	}
	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2{ X - other.X, Y - other.Y };
	}
	Vector2 Vector2::operator*(int scalar) const
	{
		return Vector2{ X * scalar, Y * scalar };
	}
	int Vector2::DistanceSquared(const Vector2& other) const
	{
		int dx = X - other.X;
		int dy = Y - other.Y;
		return dx * dx + dy * dy;
	}
	float Vector2::Distance(const Vector2& other) const
	{
		return std::sqrt(static_cast<float>(DistanceSquared(other)));
	}
	bool Vector2::operator<(const Vector2& other) const
	{
		if (X < other.X) return true;
		if (X > other.X) return false;
		return Y < other.Y;
	}
}