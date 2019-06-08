#pragma once
#include <math.h>

struct v
{
	float x;
	float y;
	float z;

	v();
	v(float x, float y, float z);
	v Cross(v other);
	float Magnitude();
	v Normalized();

	static const v f;
	static const v u;
	static const v r;
	static const v zero;
};

v operator+(const v& a, const v& b);
void operator+=(v& a, const v& b);
v operator-(const v& a, const v& b);
void operator-=(v& a, const v& b);
v operator*(const v& a, const float& factor);
v operator*(const v& a, const v& b);
v operator-(const v& theV);