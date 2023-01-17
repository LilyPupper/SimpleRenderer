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

	const char* m_MeshID;

	std::string m_Name;
	std::vector<Vertex> m_Vertices;
	std::vector<Tri> m_Triangles;
};