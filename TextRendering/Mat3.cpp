#include "Mat3.h"

#include "Vec3.h"

#include <cassert>
#include <math.h>

Mat3::Mat3()
	: x1(0.0f), x2(0.0f), x3(0.0f), y1(0.0f), y2(0.0f), y3(0.0f), z1(0.0f), z2(0.0f), z3(0.0f)
{}

Mat3::Mat3(const float& _x1, const float& _x2, const float& _x3, const float& _y1, const float& _y2, const float& _y3, const float& _z1, const float& _z2, const float& _z3)
	: x1(_x1), x2(_x2), x3(_x3), y1(_y1), y2(_y2), y3(_y3), z1(_z1), z2(_z2), z3(_z3)
{}

Mat3::~Mat3()
{}

// TODO: Move to Mat4 class
Mat3 Mat3::BuildPerspectionProjection(const float& _fov, const float& _aspect, const float& _nearDist, const float& _farDist, const bool& _leftHanded)
{
	assert(_fov > 0.0f && _aspect != 0.0f && "bad parameters!");

	float frustumDepth = _farDist - _nearDist;
	float oneOverDepth = 1.0f / frustumDepth;

	Mat3 result;
	result[1][1] = 1.0f / tanf(0.5f * _fov);
	result[0][0] = (_leftHanded ? 1.0f : -1.0f) * result[0][0] / _aspect;
	result[2][2] = _farDist * oneOverDepth;
	result[3][2] = (-_farDist * _nearDist) * oneOverDepth;
	result[2][3] = 1.0f;
	result[3][3] = 0.0f;

	return result;
}

Vec3& Mat3::operator[](const unsigned int& _index)
{
	assert(_index < 3 && "_index out of range!");

	return *(reinterpret_cast<Vec3*>(this) + _index);
}

Vec3 Mat3::operator*(const Vec3& _v)
{
	Vec3 newPoint;
	Mat3 mat = (*this);

	newPoint.x = (mat[0][0] * _v.x) + (mat[0][1] * _v.y) + (mat[0][2] * _v.z);
	newPoint.y = (mat[1][0] * _v.x) + (mat[1][1] * _v.y) + (mat[1][2] * _v.z);
	newPoint.z = (mat[2][0] * _v.x) + (mat[2][1] * _v.y) + (mat[2][2] * _v.z);

	return newPoint;
}
