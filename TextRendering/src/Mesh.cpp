#include "Mesh.h"

#include "Tri.h"

#include <fstream>
#include <sstream>

Mesh::Mesh()
{}

Mesh::~Mesh()
{}

bool Mesh::Load(const char* _filePath)
{
	std::string line;
	std::ifstream file;

	file.open(_filePath);
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
				m_Name = line.substr(2, line.length() - 2);
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
				m_Vertices.push_back(Vertex(x, y, z));
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

				m_Triangles.push_back(Tri(m_Vertices[i1], m_Vertices[i2], m_Vertices[i3]));
				m_Triangles.push_back(Tri(m_Vertices[i3], m_Vertices[i4], m_Vertices[i1]));
			}
		}
		file.close();
		return true;
	}
	return false;
}

void Mesh::LoadCube()
{
	// Create vertices
	m_Vertices.push_back(Vertex(-1.f, -1.f, 1.f));	// 1
	m_Vertices.push_back(Vertex(1.f, -1.f, 1.f));	// 2
	m_Vertices.push_back(Vertex(1.f, 1.f, 1.f));	// 3
	m_Vertices.push_back(Vertex(-1.f, 1.f, 1.f));	// 4
						 
	m_Vertices.push_back(Vertex(-1.f, -1.f, -1.f));	// 5
	m_Vertices.push_back(Vertex(1.f, -1.f, -1.f));	// 6
	m_Vertices.push_back(Vertex(1.f, 1.f, -1.f));	// 7
	m_Vertices.push_back(Vertex(-1.f, 1.f, -1.f));	// 8
	
	// Building triangles
	// Front face		1	2
	// 1 - 2 - 4
	// 3 - 4 - 2		4	3
	m_Triangles.push_back(Tri(m_Vertices[0], m_Vertices[1], m_Vertices[3]));
	m_Triangles.push_back(Tri(m_Vertices[2], m_Vertices[3], m_Vertices[1]));
	// Right face		2	6
	// 2 - 6 - 3
	// 7 - 3 - 6		3	7
	m_Triangles.push_back(Tri(m_Vertices[1], m_Vertices[5], m_Vertices[2]));
	m_Triangles.push_back(Tri(m_Vertices[6], m_Vertices[2], m_Vertices[5]));
	// Back face		6	5
	// 6 - 5 - 7
	// 8 - 7 - 5		7	8
	m_Triangles.push_back(Tri(m_Vertices[5], m_Vertices[4], m_Vertices[6]));
	m_Triangles.push_back(Tri(m_Vertices[7], m_Vertices[6], m_Vertices[4]));
	// Left face		5	1
	// 5 - 1 - 8
	// 4 - 8 - 1		8	4
	m_Triangles.push_back(Tri(m_Vertices[4], m_Vertices[0], m_Vertices[7]));
	m_Triangles.push_back(Tri(m_Vertices[3], m_Vertices[7], m_Vertices[0]));
	// Top face			5	6
	// 5 - 6 - 1
	// 2 - 1 - 6		1	2
	m_Triangles.push_back(Tri(m_Vertices[4], m_Vertices[5], m_Vertices[0]));
	m_Triangles.push_back(Tri(m_Vertices[1], m_Vertices[0], m_Vertices[5]));
	// Bottom face		4	3
	// 4 - 3 - 8
	// 7 - 8 - 3		8	7
	m_Triangles.push_back(Tri(m_Vertices[3], m_Vertices[2], m_Vertices[7]));
	m_Triangles.push_back(Tri(m_Vertices[6], m_Vertices[7], m_Vertices[2]));
}