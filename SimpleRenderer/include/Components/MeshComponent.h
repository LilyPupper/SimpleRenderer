#pragma once

#include "Component.h"

#include <string>

class RendererBase;

class MeshComponent : public Component
{
public:
	MeshComponent(Object* _owner, RendererBase* _renderer, const std::string& meshID);
	~MeshComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	inline const std::string& GetMeshID() const { return MeshID; }

private:
	bool bVisible;
	std::string MeshID;

	RendererBase* _Renderer;
};