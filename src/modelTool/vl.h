#pragma once
#include <math.h>

#define PI 3.14159265358979323846264

struct vec
{
	float x;
	float y;
	float z;

	vec();
	vec(float x, float y, float z);
	vec Cross(const vec& other) const;
	float Dot(const vec& other) const;
	float Magnitude() const;
	vec Normalized() const;
	void Normalize();

	static const vec forward;
	static const vec up;
	static const vec right;
	static const vec zero;

	vec operator+(const vec& b) const;
	vec operator-(const vec& b) const;
	vec operator-() const;
	void operator+=(const vec& b);
	void operator-=(const vec& b);
	vec operator*(float factor) const;
	vec operator*(const vec& b) const;
	bool operator==(const vec& b) const;
};
