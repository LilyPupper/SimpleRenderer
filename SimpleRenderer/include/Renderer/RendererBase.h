#pragma once

#include "glm/glm.hpp"

#include <Windows.h>
#include <map>
#include <vector>

#include "FileLoader.h"

class Mesh;
class TransformComponent;

enum RENDER_MODE
{
	Rasterize,
	Raytrace
};

class RendererBase
{
protected:
	RendererBase(const unsigned int _width, const unsigned int _height)
		: Width(_width)
		, Height(_height)
	{}

public:
	virtual ~RendererBase() = default;

	virtual void Render(const float& _deltaTime) = 0;

	void DrawMesh(const char* _modelReference, TransformComponent* const _transform)
	{
		ObjectsToRender.push_back(std::make_pair(_modelReference, _transform));
	}

	const char* RegisterMesh(const char* _meshPath)
	{
		Mesh* mesh = RegisteredModels[_meshPath];
		if (mesh == nullptr)
		{
			mesh = FileLoader::LoadMesh(_meshPath);

			if (mesh == nullptr) return NULL;

			RegisteredModels[_meshPath] = mesh;
		}

		return _meshPath;
	}

protected:
	static void ShowConsole()
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	}

	static void HideConsole()
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

protected:
	int Width, Height;

	std::vector<std::pair<const char*, TransformComponent* const>> ObjectsToRender;

	std::map<const char*, Mesh*> RegisteredModels;

	RENDER_MODE RenderMode;
	bool bMultithreaded;
};
