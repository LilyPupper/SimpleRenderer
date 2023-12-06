#pragma once

#include <string>

#include "glm/glm.hpp"

#include <string>
#include <vector>

class Mesh;

class FileLoader
{
	struct Face
	{
		std::tuple<int, int, int> f1, f2, f3;
	};

public:
	// Caller owns Mesh*
	static Mesh* LoadMesh(const std::string& _meshPath);

private: 
	static void LoadObjData(const std::string& _meshPath, std::string& _name, std::vector<glm::vec3>& _vertices, std::vector<glm::vec2>& _UVs, std::vector<glm::vec3>& _normals, std::vector<Face>& _faces);
};