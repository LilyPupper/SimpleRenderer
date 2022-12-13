#pragma once

#include "Vec3.h"

class Tri
{
public:
	Tri(const Vec3& _v1, const Vec3& _v2, const Vec3& _v3, const short& _color = 0);
	~Tri();

	void RotatePoints(Vec3 _axis, float _theta);

	Vec3 GetSurfaceNormal() const;

	void operator=(const Tri& _other);
	Tri operator+(const Vec3& _v) const;
	Tri operator*(const float& _value) const;

	Vec3 v1;
	Vec3 v2;
	Vec3 v3;

	short Color;
};
