#pragma once

#include "Mesh.h"
#include "glm/glm.hpp"

#include "Windows.h"

class Tri
{
public:
	Tri(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3);
	~Tri();
	
	void RecalculateSurfaceNormal();
	glm::vec3 GetSurfaceNormal() const;
	
	Tri operator*(glm::mat4& _m) const;
	Tri operator*(const glm::mat4& _m) const;
	void operator*=(const glm::mat4& _m);

	WORD Colour;

	Vertex v1;
	Vertex v2;
	Vertex v3;

	bool Discard = false;

private:
	glm::vec3 SurfaceNormal;
};
