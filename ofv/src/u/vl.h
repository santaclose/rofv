#pragma once
#include <math.h>

struct vec
{
	float x;
	float y;
	float z;

	vec();
	vec(float x, float y, float z);
	vec Cross(vec other);
	float Magnitude();
	vec Normalized();
	void Normalize();

	static const vec forward;
	static const vec up;
	static const vec right;
	static const vec zero;
};

vec operator+(const vec& a, const vec& b);
void operator+=(vec& a, const vec& b);
vec operator-(const vec& a, const vec& b);
void operator-=(vec& a, const vec& b);
vec operator*(const vec& a, const float& factor);
vec operator*(const vec& a, const vec& b);
vec operator-(const vec& theV);