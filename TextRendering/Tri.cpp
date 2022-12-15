#include "Tri.h"

#include <math.h>

Tri::Tri(const Vec3& _v1, const Vec3& _v2, const Vec3& _v3)
	: v1(_v1), v2(_v2), v3(_v3)
{}

Tri::~Tri()
{}

void Tri::RotatePoints(Vec3 _axis, float _theta)
{
	v1.Rotate(_axis, _theta);
	v2.Rotate(_axis, _theta);
	v3.Rotate(_axis, _theta);
}

Vec3 Tri::GetSurfaceNormal() const
{
	return Vec3::Cross(v1 - v2, v2 - v3).Normalized();
}

void Tri::operator=(const Tri& _other)
{
	v1 = _other.v1;
	v2 = _other.v2;
	v3 = _other.v3;
}

Tri Tri::operator+(const Vec3& _v) const
{
	return Tri(v1 + _v, v2 + _v, v3 + _v);
}

Tri Tri::operator*(const float& _value) const
{
	return Tri(v1 * _value, v2 * _value, v3 * _value);
}