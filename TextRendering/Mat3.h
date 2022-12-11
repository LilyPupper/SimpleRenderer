#pragma once

class Vec3;

class Mat3
{
public:
	Mat3();
	Mat3(const float& _x1, const float& _x2, const float& _x3, const float& _y1, const float& _y2, const float& _y3, const float& _z1, const float& _z2, const float& _z3);
	~Mat3();

	static inline Mat3 Identity() { return Mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f); }
	static Mat3 BuildPerspectionProjection(const float& _fov, const float& _aspect, const float& _nearDist, const float& _farDist, const bool& _leftHanded = true);

	Vec3& operator[](const unsigned int& _index);
	Vec3 operator*(const Vec3& _v);

private:
	union
	{
		struct
		{
			float x1, x2, x3;
			float y1, y2, y3;
			float z1, z2, z3;
		};
		float dataFF[3][3];
	};
};

