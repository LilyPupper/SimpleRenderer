#pragma once

#include <vector>
#include <string>
#include <glm.hpp>

class Vertex;
class Tri;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void RecalculateSurfaceNormals();

	const char* m_MeshID;

	std::string m_Name;
	std::vector<glm::vec3> m_Vertices;
	std::vector<Tri> m_Triangles;
};