#include "Vec3.h"

#include "Mat3.h"

#include <cassert>
#include <math.h>

Vec3::Vec3()
	: x(0.0f), y(0.0f), z(0.0f)
{}

Vec3::Vec3(const float& _value)
	: x(_value), y(_value), z(_value)
{}

Vec3::Vec3(const float& _x, const float& _y, const float& _z)
	: x(_x), y(_y), z(_z)
{}

Vec3::Vec3(const Vec3& _v)
	: x(_v.x), y(_v.y), z(_v.z)
{}

Vec3::~Vec3()
{}

float Vec3::Magnitude() const
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

void Vec3::Normalize()
{
	float mag = Magnitude();
	x = x / mag;
	y = y / mag;
	z = z / mag;
}

Vec3 Vec3::Normalized() const
{
	Vec3 result = *this;
	float mag = Magnitude();
	result.x = result.x / mag;
	result.y = result.y / mag;
	result.z = result.z / mag;
	return result;
}

float Vec3::Dot(const Vec3& _lhs, const Vec3& _rhs)
{
	return (_lhs.x * _rhs.x) + (_lhs.y * _rhs.y) + (_lhs.z * _rhs.z);
}

Vec3 Vec3::Cross(const Vec3& _lhs, const Vec3& _rhs)
{
	return Vec3(
		(_lhs.y * _rhs.z) - (_lhs.z * _rhs.y),
		(_lhs.z * _rhs.x) - (_lhs.x * _rhs.z),
		(_lhs.x * _rhs.y) - (_lhs.y * _rhs.x)
	);
}

void Vec3::Rotate(const Vec3& _axis, const float& _theta)
{
	float cos_theta = cosf(_theta);
	float sin_theta = sinf(_theta);

	Vec3 rotated = ((*this) * cos_theta) + (Cross(_axis, (*this)) * sin_theta) + (_axis * Dot(_axis, (*this))) * (1 - cos_theta);

	x = rotated.x;
	y = rotated.y;
	z = rotated.z;
}

void Vec3::RotateX(const float& _theta)
{
	Mat3 rotation(
		1.f, 0.f, 0.f,
		0.f, cosf(_theta), -sinf(_theta),
		0.f, sinf(_theta), cosf(_theta)
	);
	(*this) = rotation * (*this);
}

void Vec3::RotateY(const float& _theta)
{
	Mat3 rotation(
		cosf(_theta), 0.f, sinf(_theta),
		0.f, 1.f, 0.f,
		-sinf(_theta), 0.f, cosf(_theta)
	);
	(*this) = rotation * (*this);
}

void Vec3::RotateZ(const float& _theta)
{
	Mat3 rotation(
		cosf(_theta), -sinf(_theta), 0.f,
		sinf(_theta), cosf(_theta), 0.f,
		0.f, 0.f, 1.f
	);
	(*this) = rotation * (*this);
}

Vec3 Vec3::Rotate(const Vec3& _v, const Vec3& _axis, const float& _theta)
{
	float cos_theta = cosf(_theta);
	float sin_theta = sinf(_theta);

	return (_v * cos_theta) + (Cross(_axis, _v) * sin_theta) + (_axis * Dot(_axis, _v)) * (1 - cos_theta);
}

Vec3 Vec3::RotateX(const Vec3& _v, const float& _theta)
{
	Mat3 rotation(
		1.f, 0.f, 0.f,
		0.f, cosf(_theta), -sinf(_theta),
		0.f, sinf(_theta), cosf(_theta)
	);
	return rotation * _v;
}

Vec3 Vec3::RotateY(const Vec3& _v, const float& _theta)
{
	Mat3 rotation(
		cosf(_theta), 0.f, sinf(_theta),
		0.f, 1.f, 0.f,
		-sinf(_theta), 0.f, cosf(_theta)
	);
	return rotation * _v;
}

Vec3 Vec3::RotateZ(const Vec3& _v, const float& _theta)
{
	Mat3 rotation(
		cosf(_theta), -sinf(_theta), 0.f,
		sinf(_theta), cosf(_theta), 0.f,
		0.f, 0.f, 1.f
	);
	return rotation * _v;
}

Vec3 Vec3::operator+(const Vec3& _v) const
{
	return Vec3(x + _v.x, y + _v.y, z + _v.z);
}

Vec3 Vec3::operator-(const Vec3& _v) const
{
	return Vec3(x - _v.x, y - _v.y, z - _v.z);
}

Vec3 Vec3::operator*(const float& _scalar) const
{
	return Vec3(x * _scalar, y * _scalar, z * _scalar);
}

float& Vec3::operator[](const unsigned int& _index)
{
	assert(_index < 3 && "_index out of range!");

	return *(reinterpret_cast<float*>(this) + _index);
}
