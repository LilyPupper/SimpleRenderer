#pragma once

#include "Vec3.h"
#include "Tri.h"

#include <vector>
#include <string>

class Mesh
{
public:
	Mesh();
	~Mesh();

	bool Load(const char* _filePath);
	void LoadCube();

	std::string m_Name;
	std::vector<Vec3> m_Vertices;
	std::vector<Tri> m_Triangles;
};