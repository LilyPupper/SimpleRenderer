#pragma once

#include <vector>
#include <string>

#include <vec4.hpp>

class Vertex;
class Tri;

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool Load(const char* _filePath);
	void LoadCube();

	std::string m_Name;
	std::vector<Vertex> m_Vertices;
	std::vector<Tri> m_Triangles;
};