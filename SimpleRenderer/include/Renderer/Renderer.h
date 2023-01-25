#pragma once

#include "glm.hpp"

#include <map>

class Mesh;
class TransformComponent;

class Renderer
{
protected:
	Renderer() = default;
public:
	virtual ~Renderer() = default;

	virtual bool Initialise() = 0;
	virtual const char* RegisterMesh(const char* _meshPath) = 0;
	virtual void DrawMesh(const char* _modelReference, TransformComponent* const _transform) = 0;
	virtual void Flush(const float& _deltaTime) = 0;

protected:
	std::map<const char*, Mesh*> m_RegisteredModels;
};
