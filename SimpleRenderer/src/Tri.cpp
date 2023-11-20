#include "Tri.h"

#include <math.h>
#include "glm/glm.hpp"

Tri::Tri(const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3)
	: v1(glm::vec4(_v1, 1.0f)), v2(glm::vec4(_v2, 1.0f)), v3(glm::vec4(_v3, 1.0f)), m_SurfaceNormal(glm::vec4())
{}

Tri::~Tri()
{}

void Tri::RecalculateSurfaceNormal()
{
	const glm::vec3 u = v3 - v1;
	const glm::vec3 v = v2 - v1;

	m_SurfaceNormal.x = (u.y * v.z) - (u.z * v.y);
	m_SurfaceNormal.y = (u.z * v.x) - (u.x * v.z);
	m_SurfaceNormal.z = (u.x * v.y) - (u.y * v.x);

	m_SurfaceNormal = glm::normalize(m_SurfaceNormal);
}

glm::vec3 Tri::GetSurfaceNormal() const
{
	return m_SurfaceNormal;
}

Tri Tri::operator*(glm::mat4& _m) const
{
	glm::vec4 _v1 = _m * v1;
	glm::vec4 _v2 = _m * v2;
	glm::vec4 _v3 = _m * v3;

	Tri t = Tri(_v1, _v2, _v3);
	return t;
}

Tri Tri::operator*(const glm::mat4& _m) const
{
	glm::vec4 _v1 = _m * v1;
	glm::vec4 _v2 = _m * v2;
	glm::vec4 _v3 = _m * v3;

	Tri t = Tri(_v1, _v2, _v3);
	return t;
}

void Tri::operator*=(const glm::mat4& _m)
{
	v1 = _m * v1;
	v2 = _m * v2;
	v3 = _m * v3;
}