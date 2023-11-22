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

	if (pos1.y == pos3.y)
		return;

	auto GetSlopePoint = [](const glm::vec3& start, const glm::vec3& end, const int y) -> glm::vec3
	{
		float lowerX = start.x > end.x ? end.x : start.x;
		float higherX = start.x > end.x ? start.x : end.x;
		float lowerZ = start.z > end.z ? end.z : start.z;
		float higherZ = start.z > end.z ? start.z : end.z;
		float x = std::clamp(start.x + (end.x - start.x) * (y - start.y) / (end.y - start.y), lowerX, higherX);
		float z = std::clamp(start.z + (end.z - start.z) * (y - start.y) / (end.y - start.y), lowerZ, higherZ);
		return glm::vec3(x, y, z);
	};

	const int maxCapacity = std::floor(pos1.y) - std::floor(pos3.y) + 1;
	std::vector<std::pair<glm::vec3, glm::vec3>> scanlines(maxCapacity);

	bool facingLeft = pos2.x > pos3.x;

	// Get scanlines for top half of triangle
	if (std::floor(pos1.y) != std::floor(pos2.y))
	{
		for (float y = pos1.y; y >= pos2.y; --y)
		{
			glm::vec3 start = GetSlopePoint(pos2, pos1, std::floor(y));
			glm::vec3 end = GetSlopePoint(pos3, pos1, std::floor(y));

			if (start.x < end.x)
				std::swap(start, end);

			scanlines.emplace_back(std::make_pair(std::move(start), std::move(end)));
		}
	}
	
	// Get scanlines for bottom half of triangle
	if (std::floor(pos2.y) != std::floor(pos3.y))
	{
		for (float y = pos2.y; y >= pos3.y; --y)
		{
			glm::vec3 start = GetSlopePoint(pos3, pos2, std::floor(y));
			glm::vec3 end = GetSlopePoint(pos3, pos1,std::floor(y));

			if (start.x < end.x)
				std::swap(start, end);

			scanlines.emplace_back(std::make_pair(std::move(start), std::move(end)));
		}
	}
	
	// Draw scanlines
	for (std::pair<glm::vec3, glm::vec3> scanline : scanlines)
	{
		float distance = std::floor(scanline.first.x) - std::floor(scanline.second.x);

		float startDepth = scanline.first.z;
		float endDepth = scanline.second.z;
		float depthStep = (startDepth - endDepth) / distance;

		int stepCount = 0;
		for (float i = scanline.first.x; i >= scanline.first.x - distance; i -= 1.f)
		{
			int x = std::floor(i);
			int y = std::floor(scanline.first.y);

			if (x < 0 || x > Width - 1 || y < 0 || y > Height - 1)
				continue;
	
			int drawIndex = x + (y * Width);
			float z = scanline.first.z + (depthStep * stepCount);
			if (DepthData[drawIndex] > z)
			{
				currentImageData[drawIndex] = CharacterMap[index];
				DepthData[drawIndex] = z;
			}
			stepCount++;
		}
	}
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