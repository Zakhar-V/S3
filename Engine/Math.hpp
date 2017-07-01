#pragma once

#include "Base.hpp"
#include <math.h>

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//

const float Deg2Rad = 0.0174533f;
const float Rad2Deg = 57.29580406904963f;

//----------------------------------------------------------------------------//
// Vector2
//----------------------------------------------------------------------------//

struct Vector2
{
	Vector2(void) = default;
	Vector2(float _s) : x(_s), y(_s) { }
	Vector2(float _x, float _y) : x(_x), y(_y) { }

	const float operator [] (unsigned  int _index) const { return (&x)[_index]; }
	float& operator [] (unsigned  int _index) { return (&x)[_index]; }
	const float* operator * (void) const { return &x; }
	float* operator * (void) { return &x; }

	Vector2 operator - (void) const { return Vector2(-x, -y); }
	Vector2 operator + (const Vector2& _rhs) const { return Vector2(x + _rhs.x, y + _rhs.y); }
	Vector2 operator - (const Vector2& _rhs) const { return Vector2(x - _rhs.x, y - _rhs.y); }
	Vector2 operator * (const Vector2& _rhs) const { return Vector2(x * _rhs.x, y * _rhs.y); }
	Vector2 operator / (const Vector2& _rhs) const { return Vector2(x / _rhs.x, y / _rhs.y); }
	Vector2 operator * (float _rhs) const { return Vector2(x * _rhs, y * _rhs); }
	Vector2 operator / (float _rhs) const { return Vector2(x / _rhs, y / _rhs); }
	Vector2& operator += (const Vector2& _rhs) { x += _rhs.x, y += _rhs.y;	return *this; }
	Vector2& operator -= (const Vector2& _rhs) { x -= _rhs.x, y -= _rhs.y;	return *this; }
	Vector2& operator *= (const Vector2& _rhs) { x *= _rhs.x, y *= _rhs.y;	return *this; }
	Vector2& operator /= (const Vector2& _rhs) { x /= _rhs.x, y /= _rhs.y;	return *this; }
	Vector2& operator *= (float _rhs) { x *= _rhs, y *= _rhs;	return *this; }
	Vector2& operator /= (float _rhs) { x /= _rhs, y /= _rhs;	return *this; }
	friend Vector2 operator / (float _lhs, const Vector2& _rhs) { return Vector2(_lhs / _rhs.x, _lhs / _rhs.y); }
	friend Vector2 operator * (float _lhs, const Vector2& _rhs) { return Vector2(_lhs * _rhs.x, _lhs * _rhs.y); }

	bool operator == (const Vector2& _rhs) const { return x == _rhs.x && y == _rhs.y; }
	bool operator != (const Vector2& _rhs) const { return x != _rhs.x || y != _rhs.y; }

	union
	{
		struct
		{
			float x, y;
		};
		float v[2];
	};
};

//----------------------------------------------------------------------------//
// Transform
//----------------------------------------------------------------------------//

struct Transform
{
	float a = 1, b = 0, c = 0, d = 1; // 2x2
	float e = 0, f = 0; // tx, ty

	//!
	Transform(void) = default;
	//!
	Transform(float _x, float _y, float _scale, float _angle);

	//!
	Transform Copy(void) const { return *this; }
	//!
	Transform Inverse(void) const;

	//!
	Transform operator * (const Transform& _rhs) const;
	//!
	friend Vector2 operator * (const Transform& _lhs, const Vector2& _rhs);

	//!
	float Angle(void) const;
	//!
	float Scale(void) const;
	//!
	const Vector2& Pos(void) const;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
