#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Physics.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <sstream>
#include <string>

ConsoleRenderer::ConsoleRenderer(const unsigned int& _width, const unsigned int& _height)
	: m_Width(_width), m_Height(_height), m_RenderTex(_width, _height)
{}

ConsoleRenderer::~ConsoleRenderer()
{
	for (std::map<const char*, Mesh*>::iterator itr = m_RegisteredModels.begin(); itr != m_RegisteredModels.end(); itr++)
	{
		delete itr->second;
	}
}

bool ConsoleRenderer::ConsoleRenderer::Initialise()
{
	// Set console size
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect = { 0, 0, (short)m_Width, (short)m_Height };
	SetConsoleWindowInfo(hConsole, TRUE, &rect);

	// Create Screen Buffer
	m_ScreenBuffer = new wchar_t[m_Width * m_Height];
	std::fill_n(m_ScreenBuffer, m_Width * m_Height, L'a');

	m_Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (m_Console == INVALID_HANDLE_VALUE)
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		return false;
	}
	if (!SetConsoleActiveScreenBuffer(m_Console))
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		return false;
	}

	return true;
}

const char* ConsoleRenderer::LoadMesh(const char* _meshPath)
{
	Mesh* mesh = m_RegisteredModels[_meshPath];
	if (mesh == nullptr)
	{
		mesh = LoadMeshFromFile(_meshPath);

		if (mesh == nullptr) return NULL;

		m_RegisteredModels[_meshPath] = mesh;
	}

	return _meshPath;
}

const char* ConsoleRenderer::LoadCube()
{
	Mesh* mesh = m_RegisteredModels["Cube"];
	if (mesh != nullptr)
	{
		return "Cube";
	}
	
	mesh = new Mesh();

	// Create vertices
	mesh->m_Vertices.push_back(Vertex(-1.f, -1.f, 1.f));	// 1
	mesh->m_Vertices.push_back(Vertex(1.f, -1.f, 1.f));	// 2
	mesh->m_Vertices.push_back(Vertex(1.f, 1.f, 1.f));	// 3
	mesh->m_Vertices.push_back(Vertex(-1.f, 1.f, 1.f));	// 4

	mesh->m_Vertices.push_back(Vertex(-1.f, -1.f, -1.f));	// 5
	mesh->m_Vertices.push_back(Vertex(1.f, -1.f, -1.f));	// 6
	mesh->m_Vertices.push_back(Vertex(1.f, 1.f, -1.f));	// 7
	mesh->m_Vertices.push_back(Vertex(-1.f, 1.f, -1.f));	// 8

	// Building triangles
	// Front face		1	2
	// 1 - 2 - 4
	// 3 - 4 - 2		4	3
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[0], mesh->m_Vertices[1], mesh->m_Vertices[3]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[2], mesh->m_Vertices[3], mesh->m_Vertices[1]));
	// Right face		2	6
	// 2 - 6 - 3
	// 7 - 3 - 6		3	7
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[1], mesh->m_Vertices[5], mesh->m_Vertices[2]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[6], mesh->m_Vertices[2], mesh->m_Vertices[5]));
	// Back face		6	5
	// 6 - 5 - 7
	// 8 - 7 - 5		7	8
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[5], mesh->m_Vertices[4], mesh->m_Vertices[6]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[7], mesh->m_Vertices[6], mesh->m_Vertices[4]));
	// Left face		5	1
	// 5 - 1 - 8
	// 4 - 8 - 1		8	4
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[4], mesh->m_Vertices[0], mesh->m_Vertices[7]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[3], mesh->m_Vertices[7], mesh->m_Vertices[0]));
	// Top face			5	6
	// 5 - 6 - 1
	// 2 - 1 - 6		1	2
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[4], mesh->m_Vertices[5], mesh->m_Vertices[0]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[1], mesh->m_Vertices[0], mesh->m_Vertices[5]));
	// Bottom face		4	3
	// 4 - 3 - 8
	// 7 - 8 - 3		8	7
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[3], mesh->m_Vertices[2], mesh->m_Vertices[7]));
	mesh->m_Triangles.push_back(Tri(mesh->m_Vertices[6], mesh->m_Vertices[7], mesh->m_Vertices[2]));

	m_RegisteredModels["Cube"] = mesh;
	return "Cube";
}

void ConsoleRenderer::DrawMesh(const char* _modelReference, const glm::mat4& _modelMatrix)
{
	Mesh* mesh = m_RegisteredModels[_modelReference];

	if (m_Mode == RENDER_MODE::SINGLE)
		Render_Single(mesh, _modelMatrix);
	else
		Render_Async(mesh, _modelMatrix);
}

void ConsoleRenderer::Render(const float& _deltaTime)
{
	// Border string
	std::wstring border;
	for (unsigned int i = 0; i < m_RenderTex.GetWidth(); ++i)
		border += L"-";

	// Drawing the char map
	DWORD dwBytesWritten = 0;
	for (unsigned int y = 0; y < m_RenderTex.GetWidth(); ++y)
	{
		for (unsigned int x = 0; x < m_RenderTex.GetHeight(); ++x)
		{
			COORD c;
			c.X = y;
			c.Y = x;
			if (m_RenderTex[y][x].Data == 0)
			{
				WriteToScreen(x, y, L' ');
			}
			else if (m_RenderTex[y][x].Data == 1)
			{
				WORD attr = FOREGROUND_GREEN;
				WriteConsoleOutputAttribute(m_Console, &attr, 1, c, &dwBytesWritten);
				WriteToScreen(x, y, L'#');
			}
		}
	}

	WriteToScreen(0, 0, border);
	WriteToScreen(m_RenderTex.GetHeight() - 1, 0, L"FPS: " + std::to_wstring(1.0f / _deltaTime));

	WriteConsoleOutputCharacter(m_Console, m_ScreenBuffer, m_RenderTex.GetWidth() * m_RenderTex.GetHeight(), { 0,0 }, &dwBytesWritten);
}

void ConsoleRenderer::Flush()
{
	m_RenderTex.Clear();
}

void ConsoleRenderer::Render_Single(const Mesh* _mesh, const glm::mat4& _modelMatrix)
{
	// For every pixel in the texture
	for (unsigned int y = 0; y < m_RenderTex.GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
		{
			RenderPixel(_mesh, y, x, _modelMatrix);
		}
	}
}

void ConsoleRenderer::Render_Async(const Mesh* _mesh, const glm::mat4& _modelMatrix)
{
	for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
	{
		m_Futures.push_back(std::async(std::launch::async, &ConsoleRenderer::RenderRow, this, _mesh, x, _modelMatrix));
	}

	bool finished = false;
	while (!finished)
	{
		int count = 0;
		for (unsigned int i = 0; i < m_Futures.size(); ++i)
		{
			if (m_Futures[i]._Is_ready())
				++count;
		}

		if (count == m_Futures.size())
		{
			finished = true;
			m_Futures.clear();
		}
	}
}

void ConsoleRenderer::RenderRow(const Mesh* _mesh, unsigned int _row, const glm::mat4& _modelMatrix)
{
	for (unsigned int y = 0; y < m_RenderTex.GetHeight(); ++y)
	{
		if (m_RenderTex[_row][y].Data == 1)
			continue;

		RenderPixel(_mesh, y, _row, _modelMatrix);
	}
}

void ConsoleRenderer::RenderPixel(const Mesh* _mesh, unsigned int _column, unsigned int _row, const glm::mat4& _modelMatrix)
{
	glm::vec4 origin((float)_row, (float)_column, -10.0f, 1.0f);
	glm::vec4 dir(0.0f, 0.0f, 1.0f, 0.0f);

	// Raycast every triangle
	for (unsigned int i = 0; i < _mesh->m_Triangles.size(); ++i)
	{
		// Apply transformation matrix to current tri
		Tri t = _mesh->m_Triangles[i];
		t = t * _modelMatrix;

		// Raycast
		float baryX, baryY, distance;
		if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
		{
			// Depth test
			if (distance < m_RenderTex[_row][_column].Depth)
			{
				m_RenderTex[_row][_column].Data = 1;
				m_RenderTex[_row][_column].Depth = distance;
			}
		}
	}
}

void ConsoleRenderer::WriteToScreen(int _row, int _col, wchar_t _char)
{
	m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col] = _char;
}

void ConsoleRenderer::WriteToScreen(int _row, int _col, const std::wstring& _s)
{
	swprintf(&m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col], _s.size() + 1, L"%s", _s.c_str());
}

Mesh* ConsoleRenderer::LoadMeshFromFile(const char* _meshPath)
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
		return mesh;
	}

	delete mesh;
	return nullptr;
}