#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Physics.h"

#include "Components/TransformComponent.h"

#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <limits>
#include <algorithm>
#include <execution>

#include "Objects/Camera.h"

// Undefining these macros allows the use of std::numeric_limits<>::max() and std::min without errors
#undef max
#undef min

ConsoleRenderer::ConsoleRenderer(const unsigned int _width, const unsigned int _height)
	: RendererBase(_width, _height)
	, ConsoleBuffer(nullptr)
	, Console(INVALID_HANDLE_VALUE)
	, ThreadPool(std::thread::hardware_concurrency())
{
	ShowConsole();

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
	bMultithreaded = false;

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

	for (std::map<const char*, Mesh*>::iterator itr = RegisteredModels.begin(); itr != RegisteredModels.end(); itr++)
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
			// The console window seems to size itself correctly when the width and height are below the following values
			// so if our width or height is above these limits we need to manually size the window
			constexpr int MaxWidth = 240;
			constexpr int MaxHeight = 80;

			if (Width > MaxWidth || Height > MaxHeight)
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

void ConsoleRenderer::Render(const float& _deltaTime)
{ 
	switch(RenderMode)
	{
		case Raytrace:
		{
			RenderRaycast();
			break;
		}
		case Rasterize:
		{
			RenderRasterize();
			break;
		}
	}

	ObjectsToRender.clear();

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
	const wchar_t* CharacterMap = L".:-=+*8#%@";
	
	wchar_t result = L' ';

	// For every mesh
	for (unsigned int meshIndex = 0; meshIndex < ObjectsToRender.size(); ++meshIndex)
	{
		Mesh* mesh = RegisteredModels[ObjectsToRender[meshIndex].first];
		TransformComponent* transform = ObjectsToRender[meshIndex].second;

		float lastDistance = std::numeric_limits<float>::infinity();

		// Raycast every triangle
		for (unsigned int triIndex = 0; triIndex < mesh->m_Triangles.size(); ++triIndex)
		{
			// Apply transformation matrix to current tri
			TransformComponent* cameraTransform = Camera::Main->GetTransform();
			if (!cameraTransform)
			{
				return result;
			}

			Tri transformedTri = TriangleToScreenSpace(mesh->m_Triangles[triIndex], transform);

			const glm::vec3 normal = transformedTri.GetSurfaceNormal();

			const glm::vec3 camForward = cameraTransform->GetForward();
			// Backface culling
			if (glm::dot(camForward, normal) > 0.0f)
				continue;

			// Raycast
			float baryX, baryY, distance;
			if (Physics::IntersectRayTriangle(cameraTransform->GetPosition(), camForward, transformedTri, baryX, baryY, distance))
			{
				// Depth test
				if (distance < lastDistance)
				{
					int index = 0;
					const glm::vec3 LightDir = camForward;
					float nDotL = glm::dot(LightDir, normal);
					if (nDotL < 0.0f)
					{
						index = static_cast<int>(-nDotL * 9.f) + 1;
					}
					else
					{
						//only ambient lighting
						index = 1;
					}

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
	std::fill_n(DepthData, Width * Height, std::numeric_limits<float>::max());

	if (bMultithreaded)
	{
		std::vector<unsigned int> m_ObjectIter;
		m_ObjectIter.resize(ObjectsToRender.size());
		for (unsigned int i = 0; i < ObjectsToRender.size(); ++i)
			m_ObjectIter[i] = i;

		std::for_each(std::execution::par, m_ObjectIter.begin(), m_ObjectIter.end(),
			[this](unsigned int meshIndex)
			{
				Mesh* mesh = RegisteredModels[ObjectsToRender[meshIndex].first];
				TransformComponent* transform = ObjectsToRender[meshIndex].second;

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
	}
	// Single-threaded
	else
	{
		for (unsigned int meshIndex = 0; meshIndex < ObjectsToRender.size(); ++meshIndex)
		{
			Mesh* mesh = RegisteredModels[ObjectsToRender[meshIndex].first];
			TransformComponent* transform = ObjectsToRender[meshIndex].second;
		
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

	const wchar_t* CharacterMap = L".:-=+*8#%@";

	TransformComponent* cameraTransform = Camera::Main->GetTransform();
	if (!cameraTransform)
	{
		return;
	}
	const glm::vec3 camForward = cameraTransform->GetForward();

	Tri transformedTri = TriangleToScreenSpace(_tri, _transform);
	glm::vec4& pos1 = transformedTri.v1;
	glm::vec4& pos2 = transformedTri.v2;
	glm::vec4& pos3 = transformedTri.v3;

	// Apply transformation matrix to current tri
	glm::vec3 normal = transformedTri.GetSurfaceNormal();

	// Backface culling
	if (glm::dot(camForward, glm::vec3{normal}) < 0.0f)
		return;

	// Calculate lighting
	int index = 0;
	const glm::vec3 LightDir = camForward;// (normalize(glm::vec3{-.25f, -.25f, 1.0f}));
	float nDotL = glm::dot(LightDir, normal);
	if (nDotL < 0.0f)
	{
		index = static_cast<int>(-nDotL * 9.f) + 1;
	}
	else
	{
		//only ambient lighting
		index = 1;
	}

	assert(index >= 0 && index <= 10 && "index must be in the range of 0-10");

	// Calculate lines and triangle direction
	OrderPointsByYThenX(pos2, pos1);
	OrderPointsByYThenX(pos3, pos2);
	OrderPointsByYThenX(pos2, pos1);

	int direction = (pos2.y - pos1.y) * (pos3.x - pos1.x) < (pos2.x - pos1.x) * (pos3.y - pos1.y) ? 1 : -1;

	std::vector<glm::vec3> topSmallSide = PlotLine(pos2, pos1);
	std::vector<glm::vec3> bottomSmallSide = PlotLine(pos2, pos3);
	std::vector<glm::vec3> longSide = PlotLine(pos1, pos3);

	// Wireframe view
	std::vector<std::vector<glm::vec3>> lines = { topSmallSide, bottomSmallSide, longSide };
	for each(std::vector<glm::vec3> line in lines)
	{
		for each(glm::vec3 point in line)
		{
			if (point.x < 0 || point.x > Width - 1 || point.y < 0 || point.y > Height - 1)
				continue;
	
			const int x = point.x;
			const int y = point.y;
			const int drawIndex = point.x + (point.y * Width);
			const wchar_t c = CharacterMap[index];
			currentImageData[drawIndex] = c;
		}
	}
	return;

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
		for (int smallIndex = 1; smallIndex < bottomSmallSide.size() && longIndex < longSide.size();)
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

	// Estimate depth between start and end point
	const float difference = _p0.z - _p1.z;
	const float step = difference / static_cast<float>(points.size());
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i].z = _p0.z - (step * static_cast<float>(i));
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

glm::mat4 ConsoleRenderer::GetMVP(TransformComponent* _transform) const
{
	glm::mat4 modelView = glm::inverse(Camera::GetCameraMatrix()) * _transform->GetTransformation();
	glm::mat4 projection = glm::perspective(135.0f, (float)Width / (float)Height, 0.0f, 1000.f);
	glm::vec4 viewport(0.0f, 0.0f, Width, Height);
	glm::mat4 mvp = projection * modelView;

	return mvp;
}

Tri ConsoleRenderer::TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform) const
{
	glm::vec4 p1(_tri.v1);
	glm::vec4 p2(_tri.v2);
	glm::vec4 p3(_tri.v3);

	p1.w = 1.f;
	p2.w = 1.f;
	p3.w = 1.f;

	glm::mat4 MVP = GetMVP(_transform);

	glm::vec4 pos1 = MVP * p1;
	glm::vec4 pos2 = MVP * p2;
	glm::vec4 pos3 = MVP * p3;

	pos1.x = (pos1.x + 1.0f) * (float)Width / 2;
	pos2.x = (pos2.x + 1.0f) * (float)Width / 2;
	pos3.x = (pos3.x + 1.0f) * (float)Width / 2;

	pos1.y = (pos1.y + 1.0f) * (float)Height / 2;
	pos2.y = (pos2.y + 1.0f) * (float)Height / 2;
	pos3.y = (pos3.y + 1.0f) * (float)Height / 2;

	Tri transformedTri(pos1, pos2, pos3);
	transformedTri.RecalculateSurfaceNormal();

	return transformedTri;
}

Tri ConsoleRenderer::TriangleToWorldSpace(const Tri& _tri, TransformComponent* _transform) const
{
	glm::vec4 p1(_tri.v1);
	glm::vec4 p2(_tri.v2);
	glm::vec4 p3(_tri.v3);

	glm::mat4 inverseMVP = glm::inverse(GetMVP(_transform));

	glm::vec4 pos1 = inverseMVP * p1;
	glm::vec4 pos2 = inverseMVP * p2;
	glm::vec4 pos3 = inverseMVP * p3;

	Tri transformedTri(pos1, pos2, pos3);
	transformedTri.RecalculateSurfaceNormal();

	return transformedTri;
}