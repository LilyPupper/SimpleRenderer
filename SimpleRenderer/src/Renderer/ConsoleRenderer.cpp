#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Physics.h"
#include "FileLoader.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>

ConsoleRenderer::ConsoleRenderer(const unsigned int& _width, const unsigned int& _height)
	: m_Width(_width), m_Height(_height), m_RenderTex(_width, _height), m_ScreenBuffer(nullptr), m_Console(INVALID_HANDLE_VALUE)
{
	srand(1);
}

ConsoleRenderer::~ConsoleRenderer()
{
	for (std::map<const char*, Mesh*>::iterator itr = m_RegisteredModels.begin(); itr != m_RegisteredModels.end(); itr++)
	{
		delete itr->second;
	}

	if (m_ScreenBuffer)
		delete[] m_ScreenBuffer;
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

const char* ConsoleRenderer::RegisterMesh(const char* _meshPath)
{
	Mesh* mesh = m_RegisteredModels[_meshPath];
	if (mesh == nullptr)
	{
		mesh = FileLoader::LoadMesh(_meshPath);

		if (mesh == nullptr) return NULL;

		m_RegisteredModels[_meshPath] = mesh;
	}

	return _meshPath;
}

void ConsoleRenderer::DrawMesh(const char* _modelReference, const glm::mat4& _modelMatrix)
{
	Mesh* mesh = m_RegisteredModels[_modelReference];

	if (m_Mode == RENDER_MODE::SINGLE)
		Render_Single(mesh, _modelMatrix);
	else
		Render_Async(mesh, _modelMatrix);
}

void ConsoleRenderer::Flush(const float& _deltaTime)
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
			else
			{
				WORD attr = FOREGROUND_GREEN;
				WriteConsoleOutputAttribute(m_Console, &attr, 1, c, &dwBytesWritten);
				WriteToScreen(x, y, m_RenderTex[y][x].Data);
			}
		}
	}

	WriteToScreen(0, 0, border);
	WriteToScreen(m_RenderTex.GetHeight() - 1, 0, L"FPS: " + std::to_wstring(1.0f / _deltaTime));

	WriteConsoleOutputCharacter(m_Console, m_ScreenBuffer, m_RenderTex.GetWidth() * m_RenderTex.GetHeight(), { 0,0 }, &dwBytesWritten);

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
	for (unsigned int y = 0; y < m_RenderTex.GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
		{
			m_Futures.push_back(std::async(std::launch::async, &ConsoleRenderer::RenderPixel, this, _mesh, y, x, _modelMatrix));
		}
	}

	//for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
	//{
	//	m_Futures.push_back(std::async(std::launch::async, &ConsoleRenderer::RenderRow, this, _mesh, x, _modelMatrix));
	//}

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
		RenderPixel(_mesh, y, _row, _modelMatrix);
	}
}

void ConsoleRenderer::RenderPixel(const Mesh* _mesh, unsigned int _column, unsigned int _row, const glm::mat4& _modelMatrix)
{
	const static glm::vec3 LightPos(0.0f, 0.0f, -100.0f);
	const static wchar_t* CharacterMap = L" .:-=+*#%@";

	glm::vec3 origin((float)_row, (float)_column, 0.0f);
	glm::vec3 dir(0.0f, 0.0f, 1.0f);

	// Raycast every triangle
	for (unsigned int i = 0; i < _mesh->m_Triangles.size(); ++i)
	{
		// Apply transformation matrix to current tri
		m_Lock.lock();
		Tri t = _mesh->m_Triangles[i];
		t = t * _modelMatrix;
		glm::vec3 normal = t.GetSurfaceNormal();
		m_Lock.unlock();

		// Backface culling
		if (glm::dot(dir, normal) > 0.0f)
			continue;

		// Raycast
		float baryX, baryY, distance;
		if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
		{
			// Depth test
			if (distance < m_RenderTex[_row][_column].Depth)
			{
				glm::vec3 bounceDir = glm::normalize(((-2.0f * glm::dot(normal, dir)) * normal) + dir);
				glm::vec3 hitLocation = origin + (dir * distance);
				glm::vec3 dirToLight = glm::normalize(LightPos - hitLocation);
				float lightAngle = glm::dot(dirToLight, bounceDir);
				
				int index = (int)(lightAngle * 10.0f);
				
				if (index < 0) index = 0;

				m_Lock.lock();
				m_RenderTex[_row][_column].Data = CharacterMap[index];
				m_RenderTex[_row][_column].Depth = distance;
				m_Lock.unlock();
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