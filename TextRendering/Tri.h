#pragma once

#include <glm.hpp>
#include <vec3.hpp>
#include <vec4.hpp>

#include "Vertex.h"

class Tri
{
public:
	Tri(const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3);
	~Tri();
	
	void RecalculateSurfaceNormal();
	glm::vec3 GetSurfaceNormal() const;
	
	Tri operator*(glm::mat4& _m) const;

	Vertex v1;
	Vertex v2;
	Vertex v3;

private:
	glm::vec3 m_SurfaceNormal;
};
