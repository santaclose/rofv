#include "vl.h"

v::v()
{
	x = 0;
	y = 0;
	z = 0;
}
v::v(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
v v::Cross(const v other)
{
	// i j k
	// x y z
	// o p q

	// iyq + kxp + jzo - izp - jxq - kyo

	v res;
	res.x = y * other.z - z * other.y;
	res.y = z * other.x - x * other.z;
	res.z = x * other.y - y * other.x;
	return res;
}
float v::Magnitude()
{
	return sqrt(x*x + y * y + z * z);
}
v v::Normalized()
{
	v res;
	float mag = Magnitude();
	res.x = x / mag;
	res.y = y / mag;
	res.z = z / mag;
	return res;
}

void v::Normalize()
{
	float mag = Magnitude();
	x /= mag;
	y /= mag;
	z /= mag;
}

const v v::f = v(0, 0, -1);
const v v::u = v(0, 1, 0);
const v v::r = v(1, 0, 0);
const v v::zero = v(0, 0, 0);

v operator+(const v& a, const v& b)
{
	v res(a.x + b.x, a.y + b.y, a.z + b.z);
	return res;
}

void operator+=(v& a, const v& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
}

v operator-(const v& a, const v& b)
{
	v res(a.x - b.x, a.y - b.y, a.z - b.z);
	return res;
}
void operator-=(v& a, const v& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
}

v operator*(const v& a, const float& factor)
{
	v res(a.x * factor, a.y * factor, a.z * factor);
	return res;
}

v operator*(const v& a, const v& b)
{
	v res(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	return res;
}

v operator-(const v& theV)
{
	v res(theV.x * -1, theV.y * -1, theV.z * -1);
	return res;
}