#include "Renderer/SDLRenderer.h"

#include <algorithm>
#include <execution>
#include <Objects/Camera.h>

#include "Util.h"
#include "Components/TransformComponent.h"

SDLRenderer::SDLRenderer(const unsigned int& _width, const unsigned int& _height)
	: RendererBase(_width, _height)
{
#ifdef NDEBUG
	HideConsole();
#else
	ShowConsole();
#endif

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(Width, Height, 0, &Window, &Renderer);

	WindowHandle = (HWND)SDL_GetProperty(SDL_GetWindowProperties(Window), "SDL.window.win32.hwnd", nullptr);

	bMultithreaded = false;
}

SDLRenderer::~SDLRenderer()
{
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

void SDLRenderer::Render(const float& _deltaTime)
{
	const Camera* mainCamera = Camera::Main;
	if (!mainCamera)
	{
		return;
	}

	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
	SDL_RenderClear(Renderer);

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

	// Draw a red line
	//SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    //for (unsigned int i = 0; i < Width; ++i)
    //    SDL_RenderPoint(Renderer, i, i);

	// We're not taking input just allowing the window to be moved around so it's probably ok to poll on the render thread
	SDL_PollEvent(&Event);

	SDL_RenderPresent(Renderer);
}


void SDLRenderer::RasterizeTri(const Tri& _tri, TransformComponent* const _transform)
{
	if (Camera* mainCamera = Camera::Main)
	{
		Tri transformedTri = mainCamera->TriangleToScreenSpace(_tri, _transform);
		if (transformedTri.Discard)
		{
			transformedTri.Discard = false;
			return;
		}
		glm::mat4 asciiStretchReduction = {
			3.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		transformedTri *= asciiStretchReduction;

		DrawTriangleToScreen(_tri, transformedTri, _transform);
	}
}

void SDLRenderer::DrawTriangleToScreen(const Tri& _tri, const Tri& _screenSpaceTri, TransformComponent* const _transform)
{
	// Calculate lighting per tri
	glm::vec3 lightPos{0.f, 10.f, 0.f};
	Tri worldSpaceTri = _tri * _transform->GetTransformation();
	worldSpaceTri.RecalculateSurfaceNormal();
	glm::vec3 avgPos = (worldSpaceTri.v1.Position + worldSpaceTri.v2.Position + worldSpaceTri.v3.Position) / 3.f;
	glm::vec3 lightDir = glm::normalize(avgPos - lightPos);
	glm::vec3 camToTri = glm::normalize(Camera::Main->GetTransform()->GetPosition() - avgPos);
	glm::vec3 bounce = glm::reflect(camToTri, worldSpaceTri.GetSurfaceNormal());
	float lightIntensity = glm::dot(lightDir, bounce);

	if (lightIntensity < 0)
		lightIntensity = 0.f;

	Tri screenSpacePosition = _screenSpaceTri;

	// Calculate lines and triangle direction
	Util::OrderVerticesByYThenX(screenSpacePosition.v2, screenSpacePosition.v1);
	Util::OrderVerticesByYThenX(screenSpacePosition.v3, screenSpacePosition.v2);
	Util::OrderVerticesByYThenX(screenSpacePosition.v2, screenSpacePosition.v1);

	glm::vec4& pos1 = screenSpacePosition.v1.Position;
	glm::vec4& pos2 = screenSpacePosition.v2.Position;
	glm::vec4& pos3 = screenSpacePosition.v3.Position;

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
			int x = std::floor(i);
			int y = std::floor(scanline.first.y);

			if (x < 0 || x > Width - 1 || y < 0 || y > Height - 1)
				continue;
	
			int drawIndex = x + (y * Width);
			float z = scanline.first.z + (depthStep * stepCount);

			SDL_SetRenderDrawColor(Renderer, lightIntensity * 255, lightIntensity * 255, lightIntensity * 255, 255);
			SDL_RenderPoint(Renderer, x, y);

			//if (DepthData[drawIndex] > z)
			//{
			//	currentImageData[drawIndex] = CharacterToDraw;
			//	DepthData[drawIndex] = z;
			//}
			stepCount++;
		}
	}
}