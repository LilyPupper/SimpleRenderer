#include "FileLoader.h"

#include "Mesh.h"
#include "Tri.h"

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
		int normalsLoaded = 0;
		int normalErrors = 0;

		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string firstWord;
			iss >> firstWord;

			if (firstWord == "o")
			{
				// Load name
				mesh->Name = line.substr(2, line.length() - 2);
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
				mesh->Vertices.emplace_back(glm::vec4(x, y, z, 1.f));
			}
			else if (firstWord == "vn")
			{
				if (normalsLoaded < mesh->Vertices.size())
				{
					std::string nextWord;
					iss >> nextWord;
					float x = std::stof(nextWord);
					iss >> nextWord;
					float y = std::stof(nextWord);
					iss >> nextWord;
					float z = std::stof(nextWord);
					mesh->Vertices[normalsLoaded].Normal = glm::normalize(glm::vec4(x, y, z, 0.f));
					normalsLoaded++;
				}
				else
				{
					// Corrupt file?
					normalErrors++;
				}
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

				mesh->Triangles.emplace_back(Tri(mesh->Vertices[i1], mesh->Vertices[i2], mesh->Vertices[i3]));
				mesh->Triangles.emplace_back(Tri(mesh->Vertices[i3], mesh->Vertices[i4], mesh->Vertices[i1]));
			}
		}
		file.close();

		mesh->RecalculateSurfaceNormals();

		if (normalErrors > 0)
		{
			normalErrors = 0;
		}

		return mesh;
	}

	delete mesh;
	return nullptr;
}