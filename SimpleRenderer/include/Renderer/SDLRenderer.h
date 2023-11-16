#pragma once

#include "Renderer/Renderer.h"

class SDLRenderer : public Renderer
{
public:
	SDLRenderer(const unsigned int& _width, const unsigned int& _height);
	virtual ~SDLRenderer();

	virtual bool Initialise();
	virtual void DrawMesh(const char* _modelReference, TransformComponent* const _transform);
	virtual void Render(const float& _deltaTime);
	virtual void PushToScreen(const float& _deltaTime);
};