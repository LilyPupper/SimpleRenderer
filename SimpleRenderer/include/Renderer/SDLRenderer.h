#pragma once

#include "Tri.h"
#include "Renderer/RendererBase.h"

#include "SDL3/SDL.h"

class SDLRenderer : public RendererBase
{
public:
	SDLRenderer(const unsigned int& _width, const unsigned int& _height);
	virtual ~SDLRenderer();

	virtual void Render(const float& _deltaTime);

	void RasterizeTri(const Tri& _tri, TransformComponent* const _transform);
	void DrawTriangleToScreen(const Tri& _tri, const Tri& _screenSpaceTri, TransformComponent* const _transform);

protected:
	SDL_Event Event;
	SDL_Renderer* Renderer;
	SDL_Window* Window;
};