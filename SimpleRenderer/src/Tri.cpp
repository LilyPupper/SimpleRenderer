#include "Tri.h"

#include <math.h>
#include <glm.hpp>

Tri::Tri(const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3)
	: v1(_v1), v2(_v2), v3(_v3), m_SurfaceNormal(glm::vec3())
{}

Tri::~Tri()
{}

void Tri::RecalculateSurfaceNormal()
{
	glm::vec3 vec1 = glm::vec3(v1.x, v1.y, v1.z) - glm::vec3(v2.x, v2.y, v2.z);
	glm::vec3 vec2 = glm::vec3(v2.x, v2.y, v2.z) - glm::vec3(v3.x, v3.y, v3.z);
	m_SurfaceNormal = glm::normalize(glm::cross(vec1, vec2));
}

glm::vec3 Tri::GetSurfaceNormal() const
{
	return m_SurfaceNormal;
}

Tri Tri::operator*(glm::mat4& _m) const
{
	glm::vec3 _v1 = glm::vec4((glm::vec3)v1, 1.0f) * _m;
	glm::vec3 _v2 = glm::vec4((glm::vec3)v2, 1.0f) * _m;
	glm::vec3 _v3 = glm::vec4((glm::vec3)v3, 1.0f) * _m;
	glm::vec3 normal = glm::normalize(glm::vec4(m_SurfaceNormal, 0.0f) * _m);

	Tri t = Tri(_v1, _v2, _v3);
	t.m_SurfaceNormal = normal;
	return t;
}

Tri Tri::operator*(const glm::mat4& _m) const
{
	glm::vec3 _v1 = glm::vec4((glm::vec3)v1, 1.0f) * _m;
	glm::vec3 _v2 = glm::vec4((glm::vec3)v2, 1.0f) * _m;
	glm::vec3 _v3 = glm::vec4((glm::vec3)v3, 1.0f) * _m;
	glm::vec3 normal = glm::normalize(glm::vec4(m_SurfaceNormal, 0.0f) * _m);

	Tri t = Tri(_v1, _v2, _v3);
	t.m_SurfaceNormal = normal;
	return t;
}