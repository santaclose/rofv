#include "vl.h"
#include <algorithm>

vec::vec()
{
	x = 0;
	y = 0;
	z = 0;
}
vec::vec(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
vec vec::Cross(const vec& other) const
{
	// i j k
	// x y z
	// o p q

	// iyq + kxp + jzo - izp - jxq - kyo

	vec res;
	res.x = y * other.z - z * other.y;
	res.y = z * other.x - x * other.z;
	res.z = x * other.y - y * other.x;
	return res;
}
float vec::Dot(const vec& other) const
{
	return x * other.x + y * other.y + z * other.z;
}
float vec::Magnitude() const
{
	return sqrt(x * x + y * y + z * z);
}
vec vec::Normalized() const
{
	vec res;
	float mag = Magnitude();
	res.x = x / mag;
	res.y = y / mag;
	res.z = z / mag;
	return res;
}

void vec::Normalize()
{
	float mag = Magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

const vec vec::forward = vec(0, 0, -1);
const vec vec::up = vec(0, 1, 0);
const vec vec::right = vec(1, 0, 0);
const vec vec::zero = vec(0, 0, 0);


vec vec::operator+(const vec& b) const
{
	vec res(x + b.x, y + b.y, z + b.z);
	return res;
}

vec vec::operator-(const vec& b) const
{
	vec res(x - b.x, y - b.y, z - b.z);
	return res;
}

vec vec::operator-() const
{
	return vec(-x, -y, -z);
}

void vec::operator+=(const vec& b)
{
	x += b.x;
	y += b.y;
	z += b.z;
}
void vec::operator-=(const vec& b)
{
	x -= b.x;
	y -= b.y;
	z -= b.z;
}

vec vec::operator*(float factor) const
{
	vec res(x * factor, y * factor, z * factor);
	return res;
}

vec vec::operator*(const vec& b) const
{
	vec res(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	return res;
}

bool vec::operator==(const vec& b) const
{
	return x == b.x && y == b.y && z == b.z;
}

bool vec::operator!=(const vec& b) const
{
	return !operator==(b);
}
