#pragma once

#include "glm/glm.hpp"

#include <map>

#include "FileLoader.h"

class Mesh;
class TransformComponent;

enum RENDER_MODE
{
	Rasterize,
	Raytrace
};

class Renderer
{
protected:
	Renderer() = default;
public:
	virtual ~Renderer() = default;

	virtual bool Initialise() = 0;
	virtual void DrawMesh(const char* _modelReference, TransformComponent* const _transform) = 0;
	virtual void Render(const float& _deltaTime) = 0;
	virtual void PushToScreen(const float& _deltaTime) = 0;

	const char* RegisterMesh(const char* _meshPath)
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

protected:
	std::map<const char*, Mesh*> m_RegisteredModels;

	RENDER_MODE RenderMode;
	bool bMultithreaded;
};
