#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Physics.h"
#include "FileLoader.h"

#include "Debug/Timer.h"

#include "Components/TransformComponent.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <limits>
#include <algorithm>
#include <execution>

#include "Objects/Camera.h"

ConsoleRenderer::ConsoleRenderer(const unsigned int& _width, const unsigned int& _height)
	: Width(_width)
	, Height(_height)
	, ConsoleBuffer(nullptr)
	, Console(INVALID_HANDLE_VALUE)
	, ThreadPool(std::thread::hardware_concurrency())
{
	CurrentScreenBufferIndex = 0;
	PreviousScreenBufferIndex = 0;

	for (unsigned int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		ScreenBuffers[i] = new wchar_t[_width * _height];
	}
	DepthData = new float[_width * _height];

	ImageHorizontalIter.resize(_width);
	m_ImageVerticalIter.resize(_height);
	for (unsigned int i = 0; i < _width; ++i)
		ImageHorizontalIter[i] = i;
	for (unsigned int i = 0; i < _height; ++i)
		m_ImageVerticalIter[i] = i;

	srand(1);

	Futures.resize(std::thread::hardware_concurrency());

	bRenderThreadContinue = true;
	bIsScreenBufferReady = false;

	RenderMode = RENDER_MODE::Rasterize;
	bMultithreaded = true;

	ThreadPool.enqueue([this]()
	{
		while(bRenderThreadContinue)
		{
			while(!bIsScreenBufferReady)
			{
				Sleep(5);
			}
			PushToScreen(0.0f);
			bIsScreenBufferReady = false;
		}
	});

	Initialise();
}

ConsoleRenderer::~ConsoleRenderer()
{
	bRenderThreadContinue = false;

	for (std::map<const char*, Mesh*>::iterator itr = m_RegisteredModels.begin(); itr != m_RegisteredModels.end(); itr++)
	{
		delete itr->second;
	}

	for (unsigned int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		delete[] ScreenBuffers[i];
	}
	delete[] DepthData;
}

bool ConsoleRenderer::Initialise()
{
	// Set font size
	const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut != NULL)
	{
		CONSOLE_FONT_INFOEX fontInfo;
		fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		if (GetCurrentConsoleFontEx(hStdOut, FALSE, &fontInfo))
		{
			if (Width > 240 || Height > 80)
			{
				const float aspectRatio = Width / Height;
				const int newHeight = aspectRatio * Height;
			
				float scale = 0.f;
				if (Width >= newHeight)
				{
					scale = 240.f/Width;
				}
				else
				{
					scale = 80.f/Height;
				}
			
				fontInfo.dwFontSize.X *= scale;
				fontInfo.dwFontSize.Y *= scale;
			}
			else
			{
				fontInfo.dwFontSize.X = 16;
				fontInfo.dwFontSize.Y = 16;
			}
	
			if (!SetCurrentConsoleFontEx(hStdOut, FALSE, &fontInfo))
			{
				std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
				return false;
			}
		}
	}

	// Set terminal rows/columns
	std::string sizeCommand = "mode con cols=" + std::to_string(Width) + " lines=" + std::to_string(Height);
	system(sizeCommand.c_str());

	// Create Screen Buffer
	ConsoleBuffer = new wchar_t[Width * Height];
	std::fill_n(ConsoleBuffer, Width * Height, L'a');

	Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (Console == INVALID_HANDLE_VALUE)
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		return false;
	}
	if (!SetConsoleActiveScreenBuffer(Console))
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		return false;
	}
	COORD screenBufferSize = {(SHORT)Width, (SHORT)Height};
	if (!SetConsoleScreenBufferSize(Console, screenBufferSize))
	{
		std::cerr << "SetConsoleScreenBufferSize win error: " << GetLastError() << '\n';
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

void ConsoleRenderer::Render(const float& _deltaTime)
{ 
	switch(RenderMode)
	{
		case Rasterize:
		{
			RenderRasterize();
			break;
		}
		case Raytrace:
		{
			RenderRaycast();
			break;
		}
	}

	m_ObjectsToRender.clear();

	if(!bIsScreenBufferReady)
	{
		bIsScreenBufferReady = true;
		NextScreenBuffer();
	}
}

void ConsoleRenderer::PushToScreen(const float& _deltaTime)
{
	DWORD dwBytesWritten;
	WriteConsoleOutputCharacter(Console, GetPreviousScreenBuffer(), Width * Height, { 0,0 }, &dwBytesWritten);
}

// https://youtu.be/1KTgc2SEt50
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src
void ConsoleRenderer::RenderRaycast()
{
	wchar_t* currentScreenBuffer = GetCurrentScreenBuffer();

	if(bMultithreaded)
	{
		std::for_each(std::execution::par_unseq, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[this, &currentScreenBuffer](unsigned int y)
			{
				std::for_each(std::execution::par_unseq, ImageHorizontalIter.begin(), ImageHorizontalIter.end(),
					[this, &currentScreenBuffer, &y](unsigned int x)
					{
						currentScreenBuffer[x + y * Width] = RaycastPixel(x, y);
					});
			});

		//Futures.clear();
		//for (unsigned int y = 0; y < RenderTex.GetHeight(); ++y)
		//{
		//	for(int x = 0; x<RenderTex.GetWidth(); ++x)
		//	{
		//		Futures.push_back(ThreadPool.enqueue([this, x, y, currentScreenBuffer]()
		//		{
		//
		//				currentScreenBuffer[x + y * Width] = RaycastPixel(x, y);
		//		}));
		//	}
		//}
		//for(const std::future<void>& future : Futures)
		//{
		//	future.wait();
		//}

		//const int increment = Height / std::thread::hardware_concurrency();
		//int i=0;
		//for (unsigned int y = 0; y < Height; y+=increment)
		//{
		//	Futures[i++] = (ThreadPool.enqueue([increment, this, y, currentScreenBuffer]()
		//	{
		//
		//		for(int newY = y; newY<y+increment; newY++)
		//		{
		//			for(int x = 0; x < Width; x++)
		//			{
		//				currentScreenBuffer[x + newY * Width] = RaycastPixel(x, newY);
		//			}
		//		}
		//	}));
		//}
		//for(const auto& future : Futures)
		//{
		//	future.wait();
		//}
	}
	// Single-threaded
	else
	{
		for (unsigned int y = 0; y < Height; ++y)
		{
			for (unsigned int x = 0; x < Width; ++x)
			{
				currentScreenBuffer[x + y * Width] = RaycastPixel(x, y);
			}
		}
	}
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

void ConsoleRenderer::RenderRasterize()
{
	wchar_t* currentImageData = GetCurrentScreenBuffer();

	// Reset depth texture
	std::fill_n(currentImageData, Width * Height, 0);
	constexpr float float_max = (std::numeric_limits<float>::max());
	std::fill_n(DepthData, Width * Height, float_max);

	if (bMultithreaded)
	{
		std::vector<unsigned int> m_ObjectIter;
		m_ObjectIter.resize(m_ObjectsToRender.size());
		for (unsigned int i = 0; i < m_ObjectsToRender.size(); ++i)
			m_ObjectIter[i] = i;


		std::for_each(std::execution::par_unseq, m_ObjectIter.begin(), m_ObjectIter.end(),
			[this](unsigned int meshIndex)
			{
				Mesh* mesh = m_RegisteredModels[m_ObjectsToRender[meshIndex].first];
				TransformComponent* transform = m_ObjectsToRender[meshIndex].second;

				std::vector<unsigned int> m_TriIter;
				m_TriIter.resize(mesh->m_Triangles.size());
				for (unsigned int i = 0; i < mesh->m_Triangles.size(); ++i)
					m_TriIter[i] = i;

				std::for_each(std::execution::par_unseq, m_TriIter.begin(), m_TriIter.end(),
					[this, mesh, transform](unsigned int triIndex)
					{
						RasterizeTri(mesh->m_Triangles[triIndex], transform);
					});
			});
	}
	// Single-threaded
	else
	{
		for (unsigned int meshIndex = 0; meshIndex < m_ObjectsToRender.size(); ++meshIndex)
		{
			Mesh* mesh = m_RegisteredModels[m_ObjectsToRender[meshIndex].first];
			TransformComponent* transform = m_ObjectsToRender[meshIndex].second;
		
			for (unsigned int triIndex = 0; triIndex < mesh->m_Triangles.size(); ++triIndex)
			{
				RasterizeTri(mesh->m_Triangles[triIndex], transform);
			}
		}
	}
}

// TODO: Implement better lighting
void ConsoleRenderer::RasterizeTri(const Tri& _tri, TransformComponent* const _transform)
{
	wchar_t* currentImageData = GetCurrentScreenBuffer();

	const glm::vec3 LightDir(normalize(glm::vec3{-.25f, -.25f, 1.0f}));
	const glm::vec3 LightPos(0.0f, 0.0f, -100.0f);
	const wchar_t* CharacterMap = L".:-=+*8#%@";
	// Skew the image to offset the characters being higher than they are wide
	const glm::mat4 asciiScaleOffset(
		2.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	TransformComponent* cameraT = Camera::Main->GetTransform();
	glm::mat4 CameraMatrix = Camera::GetCameraMatrix();
	CameraMatrix = glm::inverse(Camera::GetCameraMatrix());

	glm::vec3 LookDirection(0.0f, 0.0f, 1.0f);

	// Apply transformation matrix to current tri
	glm::vec3 normal = _tri.GetSurfaceNormal();

	// Backface culling
	glm::vec4 worldSpaceNormal = _transform->GetTransform()->GetTransformation() * glm::vec4(normal, 1.0f);
	if (glm::dot(LookDirection, glm::vec3{worldSpaceNormal}) > 0.0f)
		return;

	// Calculate lighting
	int index = 0;
	float nDotL = glm::dot(LightDir, glm::vec3(worldSpaceNormal));
	if (nDotL < 0.0f)
	{
		index = static_cast<int>(-nDotL * 9.f) + 1;
	}
	else
	{
		//only ambient lighting
		index = 1;
	}

	glm::vec4 p1(_tri.v1);
	glm::vec4 p2(_tri.v2);
	glm::vec4 p3(_tri.v3);
	p1.w = 1.f;
	p2.w = 1.f;
	p3.w = 1.f;

	glm::mat4 modelView = CameraMatrix * _transform->GetTransformation();
	glm::mat4 projection = glm::perspective(135.0f, (float)Width/(float)Height, 0.0f, 1000.f);
	glm::vec4 viewport(0.0f, 0.0f, Width, Height);
	glm::mat4 mvp = projection * modelView;

	glm::vec4 pos1 = mvp * p1;
	glm::vec4 pos2 = mvp * p2;
	glm::vec4 pos3 = mvp * p3;

	pos1.x = (pos1.x + 1.0f) * (float)Width/2;
	pos2.x = (pos2.x + 1.0f) * (float)Width/2;
	pos3.x = (pos3.x + 1.0f) * (float)Width/2;

	pos1.y = (pos1.y + 1.0f) * (float)Height/2;
	pos2.y = (pos2.y + 1.0f) * (float)Height/2;
	pos3.y = (pos3.y + 1.0f) * (float)Height/2;

	//glm::vec3 pos1 = glm::project(p1, modelView, projection, viewport);
	//glm::vec3 pos2 = glm::project(p2, modelView, projection, viewport);
	//glm::vec3 pos3 = glm::project(p3, modelView, projection, viewport);

	//pos1 = _tri.v1;
	//pos2 = _tri.v2;
	//pos3 = _tri.v3;

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
	//float startDepth = t.v2.z;
	//float endDepth = t.v1.z;
	//float difference = startDepth - endDepth;
	//float step = difference / topSmallSide.size();
	//for (unsigned int i = 0; i < topSmallSide.size(); ++i)
	//{
	//	topSmallSide[i].z = startDepth - (step * (float)i);
	//}
	//
	//// bottomSmallSide
	////startDepth = t.v2.z;
	//endDepth = t.v3.z;
	//difference = startDepth - endDepth;
	//step = difference / bottomSmallSide.size();
	//for (unsigned int i = 0; i < bottomSmallSide.size(); ++i)
	//{
	//	bottomSmallSide[i].z = startDepth - (step * (float)i);
	//}
	//
	//// longSide
	//startDepth = t.v1.z;
	////endDepth = t.v3.z;
	//difference = startDepth - endDepth;
	//step = difference / longSide.size();
	//for (unsigned int i = 0; i < longSide.size(); ++i)
	//{
	//	longSide[i].z = startDepth - (step * (float)i);
	//}

	// Render the triangle
	int longIndex = 0;
	float currentY = topSmallSide[0].y;

	// Get the middle index of the long side
	while (longIndex < longSide.size() - 1 && currentY != longSide[longIndex].y)
		++longIndex;
	int longMiddleIndex = longIndex;

	// Rasterize top half of triangle
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

				if (drawX < 0 || drawX > Width - 1 || drawY < 0 || drawY > Height - 1)
					continue;

				int drawIndex = drawX + (drawY * Width);
				
				float depth = startDepth - (step * x);
				if (DepthData[drawIndex] > depth)
				{
					currentImageData[drawIndex] = CharacterMap[index];
					DepthData[drawIndex] = depth;
				}
			}
	
			while (smallIndex < topSmallSide.size() - 1 && topSmallSide[smallIndex].y <= currentY)
				++smallIndex;
	
			while (longIndex > 0 && longSide[longIndex].y <= currentY)
				--longIndex;
		}
	}

	// Rasterize bottom half of triangle
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

				if (drawX < 0 || drawX > Width - 1 || drawY < 0 || drawY > Height - 1)
					continue;

				int drawIndex = drawX + (drawY * Width);

				float depth = startDepth - (step * x);
				if (DepthData[drawIndex] > depth)
				{
					currentImageData[drawIndex] = CharacterMap[index];
					DepthData[drawIndex] = depth;
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
