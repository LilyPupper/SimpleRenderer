#include "Renderer/ConsoleRenderer.h"

#include "Tri.h"
#include "Mesh.h"
#include "Util.h"
#include "Components/TransformComponent.h"
#include "Objects/Camera.h"

#include <fstream>
#include <iostream>
#include <conio.h>
#include <limits>

#include <execution>

// Undefining these macros allows the use of std::numeric_limits<>::max() and std::min without errors
#undef max
#undef min

ConsoleRenderer::ConsoleRenderer(const unsigned int _width, const unsigned int _height)
	: RendererBase(_width, _height)
	, ConsoleBuffer(nullptr)
	, Console(INVALID_HANDLE_VALUE)
	, ThreadPool(std::thread::hardware_concurrency())
{
	CurrentScreenBufferIndex = 0;
	PreviousScreenBufferIndex = 0;

	for (int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		ScreenBuffers[i] = new wchar_t[_width * _height];
	}
	for (int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		ColourBuffers[i] = new WORD[_width * _height];
	}
	DepthData = new float[_width * _height];

	ImageHorizontalIter.resize(_width);
	ImageVerticalIter.resize(_height);
	for (int i = 0; i < _width; ++i)
		ImageHorizontalIter[i] = i;
	for (int i = 0; i < _height; ++i)
		ImageVerticalIter[i] = i;

	Futures.resize(std::thread::hardware_concurrency());

	bRenderThreadContinue = true;
	bIsScreenBufferReady = false;

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

	InitialiseConsoleWindow();


	float u, v, w;
	glm::vec3 p {3.f, 3.f, 0.f};
	glm::vec3 a {4.f ,1.f, 0.f};
	glm::vec3 b {5.f, 3.f, 0.f};
	glm::vec3 c {2.f, 4.f, 0.f};

	Util::Barycentric(p, a, b, c, u, v, w);

	p = (a * u) + (b * v) + (c * w);
	p = glm::vec3();
}

ConsoleRenderer::~ConsoleRenderer()
{
	bRenderThreadContinue = false;

	for (std::map<std::string, Mesh*>::iterator itr = RegisteredModels.begin(); itr != RegisteredModels.end(); itr++)
	{
		delete itr->second;
	}

	for (int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		delete[] ScreenBuffers[i];
	}
	for (int i = 0; i < SCREEN_BUFFER_COUNT; ++i)
	{
		delete[] ColourBuffers[i];
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
		fontInfo.FontWeight = 1000; // Bold font
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
			SetConsoleTextAttribute(hStdOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		}
	}

	// Set terminal rows/columns
	std::string sizeCommand = "mode con cols=" + std::to_string(Width) + " lines=" + std::to_string(Height);
	system(sizeCommand.c_str());

	// Create Screen Buffer
	ConsoleBuffer = new wchar_t[Width * Height];

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
	if (GetKeyState(VK_SNAPSHOT) & 0x8000)
	{
		PrintToFile();
	}

	const TransformComponent* cameraTransform = Camera::Main->GetTransform();
	if (!cameraTransform)
	{
		return;
	}

	wchar_t* currentImageData = GetCurrentScreenBuffer();
	WORD* currentColourBuffer = GetCurrentColourBuffer();

	// Reset depth texture
	std::fill_n(currentImageData, Width * Height, L' ');
	std::fill_n(currentColourBuffer, Width * Height, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
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
				m_TriIter.resize(mesh->Triangles.size());
				for (unsigned int i = 0; i < mesh->Triangles.size(); ++i)
					m_TriIter[i] = i;

				std::for_each(std::execution::par, m_TriIter.begin(), m_TriIter.end(),
					[this, mesh, transform](unsigned int triIndex)
					{
						RasterizeTri(mesh->Triangles[triIndex], transform);
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
		
			for (unsigned int triIndex = 0; triIndex < mesh->Triangles.size(); ++triIndex)
			{
				RasterizeTri(mesh->Triangles[triIndex], transform);
			}
		}
	}

	ObjectsToRender.clear();

	while(!bIsScreenBufferReady)
	{
		bIsScreenBufferReady = true;
		NextScreenBuffer();
	}
}

void ConsoleRenderer::PushToScreen(const float& _deltaTime)
{
	//DWORD dwColoursWritten;
	//WriteConsoleOutputAttribute(Console, GetPreviousColourBuffer(), Width * Height, {0, 0}, &dwColoursWritten);
	DWORD dwCharactersWritten;
	WriteConsoleOutputCharacter(Console, GetPreviousScreenBuffer(), Width * Height, { 0,0 }, &dwCharactersWritten);
}

void ConsoleRenderer::RasterizeTri(const Tri& _tri, TransformComponent* const _transform)
{
	if (Camera* mainCamera = Camera::Main)
	{
		Tri transformedTri = mainCamera->TriangleToScreenSpace(_tri, _transform);
		if (transformedTri.Discard)
		{
			transformedTri.Discard = false;
			return;
		}	

		DrawTriangleToScreen(_tri, transformedTri, _transform);
	}
}

void ConsoleRenderer::DrawTriangleToScreen(const Tri& _tri, const Tri& _screenSpaceTri, TransformComponent* const _transform)
{
	wchar_t* currentImageData = GetCurrentScreenBuffer();
	WORD* currentColourData = GetCurrentColourBuffer();

	Tri worldSpaceTri = _tri * _transform->GetTransformation();
	worldSpaceTri.RecalculateSurfaceNormal();

	// Calculate lines and triangle direction
	Tri screenSpaceTri = _screenSpaceTri;
	Util::OrderVerticesByYThenX(screenSpaceTri.v2, screenSpaceTri.v1);
	Util::OrderVerticesByYThenX(screenSpaceTri.v3, screenSpaceTri.v2);
	Util::OrderVerticesByYThenX(screenSpaceTri.v2, screenSpaceTri.v1);
	
	glm::vec4& pos1 = screenSpaceTri.v1.Position;
	glm::vec4& pos2 = screenSpaceTri.v2.Position;
	glm::vec4& pos3 = screenSpaceTri.v3.Position;

	// Early out if triangle has no area
	if (pos1.y == pos3.y)
		return;

	bool facingLeft = pos2.x > pos3.x;

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
		float distance = scanline.first.x - scanline.second.x;

		if (distance == 0.f)
		{
			continue;
		}

		float startDepth = scanline.first.z;
		float endDepth = scanline.second.z;

		if (facingLeft)
			std::swap(startDepth, endDepth);
		
		float depthStep = (startDepth - endDepth) / distance;

		int stepCount = 0;
		for (float i = scanline.first.x; i >= scanline.first.x - distance; i -= 1.f)
		{
			const float fx = i;
			const float fy = scanline.first.y;

			const int x = std::floor(fx);
			const int y = std::floor(fy);

			if (x < 0 || x > Width - 1 || y < 0 || y > Height - 1)
				continue;
	
			const int drawIndex = x + (y * Width);
			const float fz = scanline.first.z + (depthStep * stepCount);

			if (DepthData[drawIndex] > fz)
			{
				// Interpolate pixel position
				float u, v, w;
				const glm::vec3 pixelPos {fx, fy, fz};
				Util::Barycentric(pixelPos, _screenSpaceTri.v1.Position, _screenSpaceTri.v2.Position, _screenSpaceTri.v3.Position, u, v, w);

				const glm::vec3 pixelWorldPos = worldSpaceTri.v1.Position * u + worldSpaceTri.v2.Position * v + worldSpaceTri.v3.Position * w;
				const glm::vec3 pixelWorldNormal = glm::normalize(worldSpaceTri.v1.Normal * u + worldSpaceTri.v2.Normal * v + worldSpaceTri.v3.Normal * w);

				const glm::vec3 lightPos{0.f, 1000.f, 0.f};

				const glm::vec3 lightDir = glm::normalize(pixelWorldPos - lightPos);
				const float lightIntensity = glm::dot(lightDir, pixelWorldNormal);
				const wchar_t CharacterToDraw = LightIntensityToAsciiCharacter(lightIntensity);


				currentImageData[drawIndex] = CharacterToDraw;
				currentColourData[drawIndex] = _tri.Colour;
				DepthData[drawIndex] = fz;
			}
			stepCount++;
		}
	}
}

wchar_t ConsoleRenderer::LightIntensityToAsciiCharacter(const float _lightIntensity) const
{
	int index = static_cast<int>(_lightIntensity * static_cast<float>(CharacterMapLength));

	if (index < 0)
		index = 0;
	else if (index >= CharacterMapLength)
		index = CharacterMapLength - 1;

	return CharacterMap[index];
}

void ConsoleRenderer::PrintToFile(const bool _cropImage /*= true*/) const
{
	const wchar_t* currentImageData = GetCurrentScreenBuffer();

	int startX = 0, startY = 0;
	int endX = Width;
	int endY = Height;
	if (_cropImage)
	{
		int lowest = Height;
		int highest = 0;
		int leftmost = Width;
		int rightmost = 0;

		for (int y = 0; y < Height; ++y)
		{
			for(int x = 0; x < Width; ++x)
			{
				const int index = x + (y * Width);
				const wchar_t currentChar = currentImageData[index];

				if (currentChar != L' ')
				{
					if (x < leftmost)
					{
						leftmost = x;
					}
					if (x > rightmost)
					{
						rightmost = x;
					}
					if (y < lowest)
					{
						lowest = y;
					}
					if (y > highest)
					{
						highest = y;
					}
				}
				
			}
		}

		startX = leftmost;
		endX = rightmost;
		startY = lowest;
		endY = highest;
	}

	std::wofstream out("Screenshot.txt");
	
	for (int y = startY; y < endY; ++y)
	{
		for(int x = startX; x < endX; ++x)
		{
			const int index = x + (y * Width);
			out << currentImageData[index];
		}
		out << "\n";
	}
	
	out.close();
}