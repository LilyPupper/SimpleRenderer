#include "FileLoader.h"

#include "Mesh.h"
#include "Tri.h"
#include "Vertex.h"

#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>

Mesh* FileLoader::LoadMesh(const char* _meshPath)
{
	// Get current exe path
	char cCurrentPath[FILENAME_MAX];
	GetModuleFileNameA(NULL, cCurrentPath, sizeof(cCurrentPath));
	std::string directory = cCurrentPath;
	directory = directory.substr(0, directory.find_last_of("\\")) + "\\" + _meshPath;

	std::string line;
	std::ifstream file;

	Mesh* mesh = new Mesh();

	file.open(directory);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string firstWord;
			iss >> firstWord;

			if (firstWord == "o")
			{
				// Load name
				mesh->m_Name = line.substr(2, line.length() - 2);
			}
			else if (firstWord == "v")
			{
				// Load vertices
				std::string nextWord;
				iss >> nextWord;
				float x = std::stof(nextWord);
				iss >> nextWord;
				float y = std::stof(nextWord);
				iss >> nextWord;
				float z = std::stof(nextWord);
				mesh->m_Vertices.push_back(Vertex(x, y, z));
			}
			else if (firstWord == "f")
			{
				// Load indices
				std::string nextWord;
				iss >> nextWord;
				int i1 = std::stoi(nextWord) - 1;
				iss >> nextWord;
				int i2 = std::stoi(nextWord) - 1;
				iss >> nextWord;
				int i3 = std::stoi(nextWord) - 1;
				iss >> nextWord;
				int i4 = std::stoi(nextWord) - 1;

				mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[i1], mesh->m_Vertices[i2], mesh->m_Vertices[i3]));
				mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[i3], mesh->m_Vertices[i4], mesh->m_Vertices[i1]));
			}
		}
		file.close();

		mesh->RecalculateSurfaceNormals();

		return mesh;
	}

	delete mesh;
	return nullptr;
}