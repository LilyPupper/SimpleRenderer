#include "Tri.h"

#include <math.h>
#include "glm/glm.hpp"

Tri::Tri(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3)
	: v1(_v1)
	, v2(_v2)
	, v3(_v3)
	, SurfaceNormal(glm::vec4())
{}

Tri::~Tri()
{}

void Tri::RecalculateSurfaceNormal()
{
	const glm::vec3 u = v3.Position - v1.Position;
	const glm::vec3 v = v2.Position - v1.Position;

	SurfaceNormal.x = (u.y * v.z) - (u.z * v.y);
	SurfaceNormal.y = (u.z * v.x) - (u.x * v.z);
	SurfaceNormal.z = (u.x * v.y) - (u.y * v.x);

	SurfaceNormal = glm::normalize(SurfaceNormal);
}

glm::vec3 Tri::GetSurfaceNormal() const
{
	return SurfaceNormal;
}

Tri Tri::operator*(glm::mat4& _m) const
{
	const Vertex _v1(_m * v1.Position, _m * glm::vec4(v1.Normal, 0.f));
	const Vertex _v2(_m * v2.Position, _m * glm::vec4(v2.Normal, 0.f));
	const Vertex _v3(_m * v3.Position, _m * glm::vec4(v3.Normal, 0.f));

	return Tri{_v1, _v2, _v3};
}

Tri Tri::operator*(const glm::mat4& _m) const
{
	const Vertex _v1(_m * v1.Position, _m * glm::vec4(v1.Normal, 0.f));
	const Vertex _v2(_m * v2.Position, _m * glm::vec4(v2.Normal, 0.f));
	const Vertex _v3(_m * v3.Position, _m * glm::vec4(v3.Normal, 0.f));

	return Tri{_v1, _v2, _v3};
}

void Tri::operator*=(const glm::mat4& _m)
{
	v1.Position = _m * v1.Position;
	v2.Position = _m * v2.Position;
	v3.Position = _m * v3.Position;

	v1.Normal = _m * glm::vec4(v1.Normal, 0.f);
	v2.Normal = _m * glm::vec4(v2.Normal, 0.f);
	v3.Normal = _m * glm::vec4(v3.Normal, 0.f);
}