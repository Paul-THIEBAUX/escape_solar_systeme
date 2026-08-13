#pragma once

#include <algorithm>
#include <cmath>

#include "PrimitiveTypes.h"

template <typename T>
struct Vector2
{
public:
	union {
		struct {
			T x, y;
		};
		T coord[2];
	};

	constexpr Vector2() : x(0), y(0) {}

	constexpr explicit Vector2(T value) : x(value), y(value)
	{
	}

	constexpr Vector2(T x_, T y_) : x(x_), y(y_)
	{
	}

	constexpr Vector2(std::initializer_list<T> list) : x(*list.begin()), y(*(list.begin() + 1))
	{
	} 

	Vector2(const Vector2& other)
	{ 
		x = other.x;
		y = other.y;
	};

	T Norm() const
	{
		return static_cast<T>(std::sqrt(NormSquared()));
	}

	T NormSquared() const
	{
		T sum = T(0);
		sum += x * x;
		sum += y * y;
		return sum;
	};

	T Dot(const Vector2& other) const
	{ 
		T sum = T(0);
		sum += x * other.x;
		sum += y * other.y;
		return sum;
	};

	float32 Angle(const Vector2& other) const
	{ 
		T dot = Dot(other);
		T norms = Norm() * other.Norm();
		 
		return std::acos(static_cast<float32>(dot) / norms);
	};

	Vector2 Normalized()
	{
		T norm = Norm(); 

		if (norm == 0)
		{
			return *this;
		}

		return *this / norm;
	}

	float32 GetDistance(Vector2 otherPos)
	{
		float32 _x = otherPos.x - x;
		float32 _y = otherPos.y - y;

		return std::sqrt(_x * _x + _y * _y);
	}

	Vector2& operator=(const Vector2& other)
	{ 
		x = other.x;
		y = other.y;
		return *this;
	}

	Vector2 operator+(const Vector2& other) const
	{ 
		Vector2 result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}

	Vector2 operator-(const Vector2& other) const
	{
		Vector2 result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}

	Vector2 operator*(const T& scalar) const
	{ 
		Vector2 result;
		result.x = x * scalar;
		result.y = y * scalar;
		return result; 
	}

	Vector2 operator/(const T& scalar) const
	{  
		Vector2 result;
		result.x = x / scalar;
		result.y = y / scalar;
		return result;
	}

	Vector2 operator-() const
	{ 
		Vector2 result;
		result.x = -x;
		result.y = -y; 
		return result;
	}

	Vector2& operator+=(const Vector2& other)
	{ 
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& other)
	{ 
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2& operator*=(const T& scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& operator/=(const T& scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	Vector2 operator*(const Vector2& other) const
	{ 
		Vector2 result;
		result.x = x * other.x;
		result.y = y * other.y;
		return result;
	}

	Vector2 operator/(const Vector2& other) const
	{
		Vector2 result;
		result.x = x / other.x;
		result.y = y / other.y;
		return result;
	}

	Vector2& operator*=(const Vector2& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector2& operator/=(const Vector2& other)
	{
		x /= other.x;
		y /= other.y;
		return *this; 
	}

	bool operator == (const Vector2& other)
	{
		return x == other.x && y == other.y;
	}

	bool operator != (const Vector2& other)
	{
		return x != other.x || y != other.y;
	}
};

