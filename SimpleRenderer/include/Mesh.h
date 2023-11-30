#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"

class Vertex;
class Tri;

struct Vertex
{
	Vertex();
	Vertex(const glm::vec4& _position);
	Vertex(const glm::vec4& _position, const glm::vec3& _normal);

	glm::vec4 Position;
	glm::vec3 Normal;
	float LightIntensity;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void RecalculateSurfaceNormals();

	const char* MeshID;

	std::string Name;
	std::vector<Vertex> Vertices;
	std::vector<Tri> Triangles;
};