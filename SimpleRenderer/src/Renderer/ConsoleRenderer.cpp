#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Physics.h"
#include "FileLoader.h"

#include "Components/TransformComponent.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <limits>
#include <algorithm>
#include <execution>

ConsoleRenderer::ConsoleRenderer(const unsigned int& _width, const unsigned int& _height)
	: m_Width(_width), m_Height(_height), m_RenderTex(_width, _height), m_ScreenBuffer(nullptr), m_Console(INVALID_HANDLE_VALUE)
{
	m_ImageData = new wchar_t[_width * _height];
	m_DepthData = new float[_width * _height];

	m_ImageHorizontalIter.resize(_width);
	m_ImageVerticalIter.resize(_height);
	for (unsigned int i = 0; i < _width; ++i)
		m_ImageHorizontalIter[i] = i;
	for (unsigned int i = 0; i < _height; ++i)
		m_ImageVerticalIter[i] = i;

	srand(1);
}

ConsoleRenderer::~ConsoleRenderer()
{
	for (std::map<const char*, Mesh*>::iterator itr = m_RegisteredModels.begin(); itr != m_RegisteredModels.end(); itr++)
	{
		delete itr->second;
	}

	delete[] m_ScreenBuffer;
	delete[] m_ImageData;
	delete[] m_DepthData;
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

void ConsoleRenderer::DrawMesh(const char* _modelReference, TransformComponent* const _transform)
{
	m_ObjectsToRender.push_back(std::make_pair(_modelReference, _transform));
}

void ConsoleRenderer::Flush(const float& _deltaTime)
{
	// RENDER
#if defined(RENDERMODE_RAYCAST)
	RenderRaycast();
#elif defined(RENDERMODE_RASTERIZE)
	RenderRasterize();
#endif
	m_ObjectsToRender.clear();

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
	WriteToScreen(m_RenderTex.GetHeight() - 2, 0, L"MS: " + std::to_wstring((int)(_deltaTime * 1000.0f)));
	WriteToScreen(m_RenderTex.GetHeight() - 1, 0, L"FPS: " + std::to_wstring(1.0f / _deltaTime));

	WriteConsoleOutputCharacter(m_Console, m_ScreenBuffer, m_RenderTex.GetWidth() * m_RenderTex.GetHeight(), { 0,0 }, &dwBytesWritten);

	m_RenderTex.Clear();
}

// https://youtu.be/1KTgc2SEt50
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src
#pragma region RAYCAST
void ConsoleRenderer::RenderRaycast()
{
#if MULTITHREAD
	std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[this](unsigned int y)
		{
			std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](unsigned int x)
				{
					m_ImageData[x + y * m_Width] = RaycastPixel(x, y);
				});
		});
#else
	for (unsigned int y = 0; y < m_RenderTex.GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
		{
			m_ImageData[x + y * m_Width] = RaycastPixel(x, y);
		}
	}
#endif

	m_RenderTex.SetData(m_ImageData);
}

wchar_t ConsoleRenderer::RaycastPixel(unsigned int _column, unsigned int _row)
{
	const glm::vec3 LightPos(0.0f, 0.0f, -100.0f);
	const wchar_t* CharacterMap = L".:-=+*8#%@";
	// Skew the image to offset the characters being higher than they are wide
	const glm::mat4 ViewProjection(
		1.8f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	
	glm::vec3 origin((float)_column, (float)_row, 0.0f);
	glm::vec3 dir(0.0f, 0.0f, 1.0f);
	
	wchar_t result = L' ';

	// For every mesh
	for (unsigned int meshIndex = 0; meshIndex < m_ObjectsToRender.size(); ++meshIndex)
	{
		Mesh* mesh = m_RegisteredModels[m_ObjectsToRender[meshIndex].first];
		TransformComponent* transform = m_ObjectsToRender[meshIndex].second;

		float lastDistance = std::numeric_limits<float>::infinity();

		// Raycast every triangle
		for (unsigned int triIndex = 0; triIndex < mesh->m_Triangles.size(); ++triIndex)
		{
			// Apply transformation matrix to current tri
			Tri t = mesh->m_Triangles[triIndex];
			t *= transform->GetTransformation() * ViewProjection;
			glm::vec3 normal = t.GetSurfaceNormal();

			// Backface culling
			if (glm::dot(dir, normal) > 0.0f)
				continue;

			// Raycast
			float baryX, baryY, distance;
			if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
			{
				// Depth test
				if (distance < lastDistance)
				{
					glm::vec3 bounceDir = glm::normalize(((-2.0f * glm::dot(normal, dir)) * normal) + dir);
					glm::vec3 hitLocation = dir * distance;
					glm::vec3 dirToLight = glm::normalize(LightPos - hitLocation);

					float lightAngle = glm::dot(dirToLight, bounceDir);
					lightAngle *= 10.0f;
					int index = (int)(lightAngle * lightAngle * 0.1f);
					if (index < 0) index = 0;
					if (index > 10) index = 10;

					result = CharacterMap[index];
					lastDistance = distance;
				}
			}
		}
	}

	return result;
}
#pragma endregion

#pragma region RASTERIZE
void ConsoleRenderer::RenderRasterize()
{
	std::fill_n(m_ImageData, m_Width * m_Height, 0);
	constexpr float float_max = (std::numeric_limits<float>::max());
	std::fill_n(m_DepthData, m_Width * m_Height, float_max);

#if MULTITHREAD
	std::vector<unsigned int> m_ObjectIter;
	m_ObjectIter.resize(m_ObjectsToRender.size());
	for (unsigned int i = 0; i < m_ObjectsToRender.size(); ++i)
		m_ObjectIter[i] = i;


	std::for_each(std::execution::par, m_ObjectIter.begin(), m_ObjectIter.end(),
		[this](unsigned int meshIndex)
		{
			Mesh* mesh = m_RegisteredModels[m_ObjectsToRender[meshIndex].first];
			TransformComponent* transform = m_ObjectsToRender[meshIndex].second;

			std::vector<unsigned int> m_TriIter;
			m_TriIter.resize(mesh->m_Triangles.size());
			for (unsigned int i = 0; i < mesh->m_Triangles.size(); ++i)
				m_TriIter[i] = i;

			std::for_each(std::execution::par, m_TriIter.begin(), m_TriIter.end(),
				[this, mesh, transform](unsigned int triIndex)
				{
					RasterizeTri(mesh->m_Triangles[triIndex], transform);
				});
		});
#else
	for (unsigned int meshIndex = 0; meshIndex < m_ObjectsToRender.size(); ++meshIndex)
	{
		Mesh* mesh = m_RegisteredModels[m_ObjectsToRender[meshIndex].first];
		TransformComponent* transform = m_ObjectsToRender[meshIndex].second;

		for (unsigned int triIndex = 0; triIndex < mesh->m_Triangles.size(); ++triIndex)
		{
			RasterizeTri(mesh->m_Triangles[triIndex], transform);
		}
	}
#endif

	m_RenderTex.SetData(m_ImageData);
}

// TODO: Implement better lighting
void ConsoleRenderer::RasterizeTri(const Tri& _tri, TransformComponent* const _transform)
{
	const glm::vec3 LightPos(0.0f, 0.0f, -100.0f);
	const wchar_t* CharacterMap = L".:-=+*8#%@";
	// Skew the image to offset the characters being higher than they are wide
	const glm::mat4 ViewProjection(
		1.8f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	glm::vec3 LookDirection(0.0f, 0.0f, 1.0f);

	// Apply transformation matrix to current tri
	Tri t = _tri * _transform->GetTransformation() * ViewProjection;
	glm::vec3 normal = t.GetSurfaceNormal();

	// Backface culling
	if (glm::dot(LookDirection, normal) > 0.0f)
		return;

	// Calculate lighting
	float avgDist = ((t.v1 + t.v2 + t.v3) / 3.0f).length();
	glm::vec3 bounceDir = glm::normalize(((-2.0f * glm::dot(normal, LookDirection)) * normal) + LookDirection);
	glm::vec3 hitLocation = (LookDirection * avgDist);
	glm::vec3 dirToLight = glm::normalize(LightPos - hitLocation);
	
	float lightAngle = glm::dot(dirToLight, bounceDir);
	lightAngle *= 10.0f;
	int index = (int)(lightAngle * lightAngle * 0.1f);
	if (index < 0) index = 0;
	if (index > 10) index = 10;

	glm::vec4 pos1 = t.v1;
	glm::vec4 pos2 = t.v2;
	glm::vec4 pos3 = t.v3;

	// Calculate lines and triangle direction
	OrderPointsByYThenX(pos2, pos1);
	OrderPointsByYThenX(pos3, pos2);
	OrderPointsByYThenX(pos2, pos1);

	int direction = (pos2.y - pos1.y) * (pos3.x - pos1.x) < (pos2.x - pos1.x) * (pos3.y - pos1.y) ? 1 : -1;

	std::vector<glm::vec3> topSmallSide = PlotLine(pos2, pos1);
	std::vector<glm::vec3> bottomSmallSide = PlotLine(pos2, pos3);
	std::vector<glm::vec3> longSide = PlotLine(pos1, pos3);

	// Estimate depth for each line
	// topSmallSide
	float startDepth = pos2.z;
	float endDepth = pos1.z;
	float difference = startDepth - endDepth;
	float step = difference / topSmallSide.size();
	for (unsigned int i = 0; i < topSmallSide.size(); ++i)
	{
		topSmallSide[i].z = startDepth - (step * (float)i);
	}

	// bottomSmallSide
	//startDepth = pos2.z;
	endDepth = pos3.z;
	difference = startDepth - endDepth;
	step = difference / bottomSmallSide.size();
	for (unsigned int i = 0; i < bottomSmallSide.size(); ++i)
	{
		bottomSmallSide[i].z = startDepth - (step * (float)i);
	}

	// longSide
	startDepth = pos1.z;
	//endDepth = pos3.z;
	difference = startDepth - endDepth;
	step = difference / longSide.size();
	for (unsigned int i = 0; i < longSide.size(); ++i)
	{
		longSide[i].z = startDepth - (step * (float)i);
	}

	// Render the triangle
	int longIndex = 0;
	float currentY = topSmallSide[0].y;

	// Get the middle index of the long side
	while (longIndex < longSide.size() - 1 && currentY != longSide[longIndex].y)
		++longIndex;
	int longMiddleIndex = longIndex;

	if (topSmallSide[0].y != topSmallSide[topSmallSide.size() - 1].y) // If these points are equal dont draw this half of the triangle
	{
		for (int smallIndex = 0; smallIndex < topSmallSide.size() && longIndex > 0;)
		{
			glm::vec3 start = topSmallSide[smallIndex];
			glm::vec3 end = longSide[longIndex];
			currentY = std::max(start.y, end.y);
			int distance = fabsf(start.x - end.x);
	
			float startDepth = start.z;
			float endDepth = end.z;
			float difference = startDepth - endDepth;
			float step = difference / longSide.size();
			for (int x = 0; x <= distance; ++x)
			{
				int drawX = start.x + (x * direction);
				int drawY = end.y;

				if (drawX < 0 || drawX > m_Width - 1 || drawY < 0 || drawY > m_Height - 1)
					continue;

				int drawIndex = drawX + (drawY * m_Width);
				
				float depth = startDepth - (step * x);
				if (m_DepthData[drawIndex] > depth)
				{
					m_ImageData[drawIndex] = CharacterMap[index];
					m_DepthData[drawIndex] = depth;
				}
			}
	
			while (smallIndex < topSmallSide.size() - 1 && topSmallSide[smallIndex].y <= currentY)
				++smallIndex;
	
			while (longIndex > 0 && longSide[longIndex].y <= currentY)
				--longIndex;
		}
	}

	longIndex = longMiddleIndex + 1;
	if (bottomSmallSide[0].y != bottomSmallSide[bottomSmallSide.size() - 1].y)
	{
		for (int smallIndex = 1; smallIndex < bottomSmallSide.size() && longIndex < longSide.size() - 1;)
		{
			glm::vec3 start = bottomSmallSide[smallIndex];
			glm::vec3 end = longSide[longIndex];
			currentY = std::min(start.y, end.y);
			float distance = std::fabsf(start.x - end.x);
	
			float startDepth = start.z;
			float endDepth = end.z;
			float difference = startDepth - endDepth;
			float step = difference / longSide.size();
			for (int x = 0; x <= distance; ++x)
			{
				int drawX = start.x + (x * direction);
				int drawY = end.y;

				if (drawX < 0 || drawX > m_Width - 1 || drawY < 0 || drawY > m_Height - 1)
					continue;

				int drawIndex = drawX + (drawY * m_Width);

				float depth = startDepth - (step * x);
				if (m_DepthData[drawIndex] > depth)
				{
					m_ImageData[drawIndex] = CharacterMap[index];
					m_DepthData[drawIndex] = depth;
				}
			}
	
			while (smallIndex < bottomSmallSide.size() - 1 && bottomSmallSide[smallIndex].y >= currentY)
				++smallIndex;
	
			float currentEndY = longSide[longIndex].y;
			while (longIndex < longSide.size() && longSide[longIndex].y >= currentEndY)
				++longIndex;
		}
	}
}

// http://members.chello.at/~easyfilter/bresenham.html
std::vector<glm::vec3> ConsoleRenderer::PlotLine(const glm::vec3& _p0, const glm::vec3& _p1)
{
	int x0 = (int)std::roundf(_p0.x);
	int y0 = (int)std::roundf(_p0.y);
	int x1 = (int)std::roundf(_p1.x);
	int y1 = (int)std::roundf(_p1.y);

	int dx = (int)abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -(int)abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	std::vector<glm::vec3> points;

	for (; ; )
	{  /* loop */
		glm::vec3 point = glm::vec3(x0, y0, 0.0f);
		points.push_back(point);

		if (x0 == x1 && y0 == y1) break;
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
		if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
	}

	return points;
}

void ConsoleRenderer::OrderPointsByYThenX(glm::vec4& _high, glm::vec4& _low)
{
	if (_high.y >= _low.y)
	{
		if (_high.y == _low.y)
		{
			if (_high.x > _low.x)
			{
				std::swap(_high, _low);
			}
		}
		else
		{
			std::swap(_high, _low);
		}
	}
}
#pragma endregion

void ConsoleRenderer::WriteToScreen(int _row, int _col, wchar_t _char)
{
	m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col] = _char;
}

void ConsoleRenderer::WriteToScreen(int _row, int _col, const std::wstring& _s)
{
	swprintf(&m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col], _s.size() + 1, L"%s", _s.c_str());
}