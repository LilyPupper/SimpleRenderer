#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"

#include "Components/TransformComponent.h"

#include <iostream>
#include <conio.h>
#include <limits>
#include <algorithm>
#include <execution>

#include "Objects/Camera.h"

// Undefining these macros allows the use of std::numeric_limits<>::max() and std::min without errors
#undef max
#undef min

ConsoleRenderer::ConsoleRenderer(const unsigned int _width, const unsigned int _height)
	: RendererBase(_width, _height, 60.f, 0.1f, 1000.f)
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

	Futures.resize(std::thread::hardware_concurrency());

	bRenderThreadContinue = true;
	bIsScreenBufferReady = false;

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

	InitialiseConsoleWindow();
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

bool ConsoleRenderer::InitialiseConsoleWindow()
{
	ShowConsole();

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

	WindowHandle = GetConsoleWindow();

	return true;
}

void ConsoleRenderer::Render(const float& _deltaTime)
{ 
	const TransformComponent* cameraTransform = Camera::Main->GetTransform();
	if (!cameraTransform)
	{
		return;
	}

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

void ConsoleRenderer::RasterizeTri(const Tri& _tri, TransformComponent* const _transform)
{
	Tri transformedTri = TriangleToScreenSpace(_tri, _transform);
	if (transformedTri.Discard)
	{
		transformedTri.Discard = false;
		return;
	}
	
	DrawTriangleToScreen(_tri, transformedTri, _transform);
}

void Barycentric(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& u, float& v, float& w)
{
	const glm::vec3 v0 = b -a;
	const glm::vec3 v1 = c - a;
	const glm::vec3 v2 = p - a;
	const float d00 = glm::dot(v0, v0);
	const float d01 = glm::dot(v0, v1);
	const float d11 = glm::dot(v1, v1);
	const float d20 = glm::dot(v2, v0);
	const float d21 = glm::dot(v2, v1);
	const float denom = d00 * d11 - d01 * d01;
	v = (d11 * d20 - d01 * d21) / denom;
	w = (d00 * d21 - d01 * d20) / denom;
	u = 1.f -v - w;
}

// https://youtu.be/PahbNFypubE?si=WROMBcqVb14EpsfC
void ConsoleRenderer::DrawTriangleToScreen(const Tri& _worldTri, const Tri& _screenSpaceTri, TransformComponent* const _transform)
{
	wchar_t* currentImageData = GetCurrentScreenBuffer();
	const wchar_t* CharacterMap = L".:-=+*8#%@";

	// Calculate lighting
	int index = 0;
	Tri worldSpaceTri = _worldTri * _transform->GetTransformation();
	worldSpaceTri.RecalculateSurfaceNormal();
	glm::vec3 avgPos = (worldSpaceTri.v1 + worldSpaceTri.v2 + worldSpaceTri.v3) / 3.f;
	glm::vec3 lightPos{0.f, 10.f, 0.f};
	glm::vec3 lightDir = glm::normalize(avgPos - lightPos);
	glm::vec3 camToTri = glm::normalize(Camera::Main->GetTransform()->GetPosition() - avgPos);
	glm::vec3 bounce = glm::reflect(camToTri, worldSpaceTri.GetSurfaceNormal());
	float nDotL = glm::dot(lightDir, bounce);
	index = static_cast<int>(-nDotL * 9.f) + 1;
	if (nDotL < 0.0f)
	{
		index = static_cast<int>(-nDotL * 9.f) + 1;
	}
	else
	{
		// only ambient lighting
		index = 1;
	}

	//if (index < 0)
	//	index = 0;

	glm::vec4 pos1 = _screenSpaceTri.v1;
	glm::vec4 pos2 = _screenSpaceTri.v2;
	glm::vec4 pos3 = _screenSpaceTri.v3;

	// Calculate lines and triangle direction
	OrderPointsByYThenX(pos2, pos1);
	OrderPointsByYThenX(pos3, pos2);
	OrderPointsByYThenX(pos2, pos1);

	int direction = (pos2.y - pos1.y) * (pos3.x - pos1.x) < (pos2.x - pos1.x) * (pos3.y - pos1.y) ? 1 : -1;

	std::vector<glm::vec3> topSmallSide = PlotLine(pos2, pos1);
	std::vector<glm::vec3> bottomSmallSide = PlotLine(pos2, pos3);
	std::vector<glm::vec3> longSide = PlotLine(pos1, pos3);

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
			float distance = fabsf(start.x - end.x);
	
			float startDepth = start.z;
			float endDepth = end.z;
			float difference = startDepth - endDepth;
			float step = difference / longSide.size();
			for (float x = 0; x <= distance; ++x)
			{
				float drawX = start.x + (x * direction);
				float drawY = end.y;

				if (drawX < 0 || drawX > (Width - 1) || drawY < 0 || drawY > (Height - 1))
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
		for (int smallIndex = 0; smallIndex < bottomSmallSide.size() && longIndex < longSide.size();)
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

void ConsoleRenderer::DrawTriangleTopHalf(const std::vector<glm::vec3>& _longSide, const std::vector<glm::vec3>& _shortSide)
{
	//wchar_t* currentImageData = GetCurrentScreenBuffer();
	//
	//float currentY = _shortSide[0].y;
	//int longIndex = 0;
	//
	//for (int smallIndex = 0; smallIndex < _shortSide.size() && longIndex > 0;)
	//	{
	//		glm::vec3 start = _shortSide[smallIndex];
	//		glm::vec3 end = _longSide[longIndex];
	//
	//		currentY = std::max(start.y, end.y);
	//		float distance = fabsf(start.x - end.x);
	//
	//		float startDepth = start.z;
	//		float endDepth = end.z;
	//		float difference = startDepth - endDepth;
	//		float step = difference / _longSide.size();
	//		for (float x = 0; x <= distance; ++x)
	//		{
	//			float drawX = start.x + (x * direction);
	//			float drawY = end.y;
	//
	//			if (drawX < 0 || drawX > (Width - 1) || drawY < 0 || drawY > (Height - 1))
	//				continue;
	//
	//			int drawIndex = drawX + (drawY * Width);
	//			
	//			float depth = startDepth - (step * x);
	//			if (DepthData[drawIndex] > depth)
	//			{
	//				currentImageData[drawIndex] = CharacterMap[index];
	//				DepthData[drawIndex] = depth;
	//			}
	//		}
	//
	//		while (smallIndex < _shortSide.size() - 1 && _shortSide[smallIndex].y <= currentY)
	//			++smallIndex;
	//
	//		while (longIndex > 0 && longSide[longIndex].y <= currentY)
	//			--longIndex;
	//	}
}

void ConsoleRenderer::DrawTriangleBottomHalf(const std::vector<glm::vec3>& _longSide, const std::vector<glm::vec3>& _shortSide, const int _longSideMiddleIndex)
{
	
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
	glm::mat4 asciiStretchReduction = {
	3.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
	};

	const glm::mat4 view = glm::inverse(Camera::GetCameraMatrix());
	const glm::mat4 model = _transform->GetTransformation();
	const glm::mat4 projection = asciiStretchReduction * glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
	const glm::mat4 mvp = projection * view * model;

	return mvp;
}

glm::mat4 ConsoleRenderer::GetMV(TransformComponent* _transform) const
{
	const glm::mat4 view = glm::inverse(Camera::GetCameraMatrix());
	const glm::mat4 model = _transform->GetTransformation();
	const glm::mat4 mv = view * model;

	return mv;
}

Tri ConsoleRenderer::TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform) const
{
	glm::mat4 MVP = GetMVP(_transform);

	glm::vec4 pos1 = MVP * _tri.v1;
	glm::vec4 pos2 = MVP * _tri.v2;
	glm::vec4 pos3 = MVP * _tri.v3;
	
	Tri transformedTri(pos1, pos2, pos3);
	transformedTri.RecalculateSurfaceNormal();

	// Backface culling
	glm::vec3 avgPos = (pos1 + pos2 + pos3) / 3.f;
	if (glm::dot(transformedTri.GetSurfaceNormal(), avgPos) >= 0.f)
	{
		transformedTri.Discard = true;
		return transformedTri;
	}

	pos1.x /= pos1.w;
	pos2.x /= pos2.w;
	pos3.x /= pos3.w;
	
	pos1.y /= pos1.w;
	pos2.y /= pos2.w;
	pos3.y /= pos3.w;
	
	pos1.z /= pos1.w;
	pos2.z /= pos2.w;
	pos3.z /= pos3.w;

	pos1.z = (((FarPlane - NearPlane) * pos1.z) + NearPlane + FarPlane) / 2.f;
	pos2.z = (((FarPlane - NearPlane) * pos2.z) + NearPlane + FarPlane) / 2.f;
	pos3.z = (((FarPlane - NearPlane) * pos3.z) + NearPlane + FarPlane) / 2.f;

	transformedTri.v1.x = (pos1.x + 1.0f) * ((float)Width / 2);
	transformedTri.v2.x = (pos2.x + 1.0f) * ((float)Width / 2);
	transformedTri.v3.x = (pos3.x + 1.0f) * ((float)Width / 2);
	
	transformedTri.v1.y = (pos1.y + 1.0f) * ((float)Height / 2);
	transformedTri.v2.y = (pos2.y + 1.0f) * ((float)Height / 2);
	transformedTri.v3.y = (pos3.y + 1.0f) * ((float)Height / 2);

	// Frustum culling
	auto OutsideFrustum = [this](const Tri& _tri) -> bool
	{
		auto ClipVec3 = [this](const glm::vec4& _vec) -> bool
		{
			return  (_vec.x < 0) || (_vec.x > (float)Width) ||
					(_vec.y < 0) || (_vec.y > (float)Height) ||
					(_vec.z < NearPlane) || (_vec.z > FarPlane);			
		};
	
		return ClipVec3(_tri.v1) && ClipVec3(_tri.v2) && ClipVec3(_tri.v3);
	};
	
	if (OutsideFrustum(transformedTri))
	{
		transformedTri.Discard = true;
	}

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