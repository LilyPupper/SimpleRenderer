#pragma once

class Mat3;

class Vec3
{
public:
	Vec3();
	Vec3(const float& _value);
	Vec3(const float& _x, const float& _y, const float& _z);
	Vec3(const Vec3& _v);
	~Vec3();

	void Rotate(const Vec3& _axis, const float& _theta);
	void RotateX(const float& _theta);
	void RotateY(const float& _theta);
	void RotateZ(const float& _theta);

	static Vec3 Rotate(const Vec3& _v, const Vec3& _axis, const float& _theta);
	static Vec3 RotateX(const Vec3& _v, const float& _theta);
	static Vec3 RotateY(const Vec3& _v, const float& _theta);
	static Vec3 RotateZ(const Vec3& _v, const float& _theta);

	static inline Vec3 Forward()	{ return Vec3(1.0f, 0.0f, 0.0f); }
	static inline Vec3 Up()			{ return Vec3(0.0f, 1.0f, 0.0f); }
	static inline Vec3 Right()		{ return Vec3(0.0f, 0.0f, 1.0f); }

	float Magnitude() const;

	void Normalize();
	Vec3 Normalized() const;

	static float Dot(const Vec3& _lhs, const Vec3& _rhs);
	static Vec3 Cross(const Vec3& _lhs, const Vec3& _rhs);

	Vec3 operator+(const Vec3& _v) const;
	Vec3 operator-(const Vec3& _v) const;
	Vec3 operator*(const float& _scalar) const;
	float& operator[](const unsigned int& _index);
	
	float x, y, z;
};
