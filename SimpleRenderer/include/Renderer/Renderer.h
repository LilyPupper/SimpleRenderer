#pragma once

#include "glm.hpp"

#include <map>

class Mesh;

class Renderer
{
protected:
	Renderer() = default;
public:
	virtual ~Renderer() = default;

	virtual bool Initialise() = 0;
	virtual const char* LoadMesh(const char* _meshPath) = 0;
	virtual void DrawMesh(const char* _modelReference, const glm::mat4& _modelMatrix) = 0;
	virtual void Render(const float& _deltaTime) = 0;
	virtual void Flush() = 0;

protected:
	virtual Mesh* LoadMeshFromFile(const char* _meshPath) = 0;

	std::map<const char*, Mesh*> m_RegisteredModels;
};
