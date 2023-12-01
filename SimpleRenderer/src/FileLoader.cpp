#include "FileLoader.h"

#include "Mesh.h"
#include "Tri.h"

#include <Windows.h>
#include <fstream>
#include <sstream>

Mesh* FileLoader::LoadMesh(const char* _meshPath)
{
	std::string name;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> UVs;
	std::vector<glm::vec3> normals;
	std::vector<Face> faces;
	LoadObjData(_meshPath, name, vertices, UVs, normals, faces);

	Mesh* m = new Mesh();
	for (Face face : faces)
	{
		m->Triangles.emplace_back(
			Vertex{glm::vec4(vertices[std::get<0>(face.f1) - 1], 1.f), normals[std::get<2>(face.f1) - 1]},
			Vertex{glm::vec4(vertices[std::get<0>(face.f2) - 1], 1.f), normals[std::get<2>(face.f2) - 1]},
			Vertex{glm::vec4(vertices[std::get<0>(face.f3) - 1], 1.f), normals[std::get<2>(face.f3) - 1]}
		);
	}
	return m;
}


void FileLoader::LoadObjData(const char* _meshPath, std::string& _name, std::vector<glm::vec3>& _vertices, std::vector<glm::vec2>& _UVs, std::vector<glm::vec3>& _normals, std::vector<Face>& _faces)
{
	// Get current exe path
	char cCurrentPath[FILENAME_MAX];
	GetModuleFileNameA(NULL, cCurrentPath, sizeof(cCurrentPath));
	std::string directory = cCurrentPath;
	directory = directory.substr(0, directory.find_last_of("\\")) + "\\" + _meshPath;

	std::string line;
	std::ifstream file;

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
				_name = line.substr(2, line.length() - 2);
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
				_vertices.emplace_back(glm::vec4(x, y, z, 1.f));
			}
			else if (firstWord == "vt")
			{
				std::string nextWord;
				iss >> nextWord;
				float x = std::stof(nextWord);
				iss >> nextWord;
				float y = std::stof(nextWord);
				_UVs.emplace_back(glm::vec2(x, y));
			}
			else if (firstWord == "vn")
			{
				std::string nextWord;
				iss >> nextWord;
				float x = std::stof(nextWord);
				iss >> nextWord;
				float y = std::stof(nextWord);
				iss >> nextWord;
				float z = std::stof(nextWord);
				_normals.emplace_back(glm::normalize(glm::vec3(x, y, z)));
			}
			else if (firstWord == "f")
			{
				auto GetFaceInfo = [](const std::string& _data, std::tuple<int, int, int>& _f)
				{
					const std::string delimiter = "/";
					std::string data = _data;

					const std::string vectorIndexStr = data.substr(0, data.find(delimiter));
					data = data.substr(vectorIndexStr.length() + 1);

					const std::string UVIndexStr = data.substr(0, data.find(delimiter));
					data = data.substr(UVIndexStr.length() + 1);

					const std::string normalIndexStr = data;

					int vectorIndex = -1;
					int UVIndex = -1;
					int normalIndex = -1;

					if (vectorIndexStr.size() > 0)
					{
						vectorIndex = std::stoi(vectorIndexStr);
					}
					if (UVIndexStr.size() > 0)
					{
						UVIndex =  std::stoi(UVIndexStr);
					}
					if (normalIndexStr.size() > 0)
					{
						normalIndex = std::stoi(normalIndexStr);
					}

					_f = std::make_tuple(vectorIndex, UVIndex, normalIndex);
				};

				Face face;
				std::string nextWord;

				iss >> nextWord;
				GetFaceInfo(nextWord, face.f1);

				iss >> nextWord;
				GetFaceInfo(nextWord, face.f2);

				iss >> nextWord;
				GetFaceInfo(nextWord, face.f3);

				_faces.emplace_back(face);
			}
		}
		file.close();

	}
}