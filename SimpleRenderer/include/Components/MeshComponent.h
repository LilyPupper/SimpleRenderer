#pragma once

#include "Component.h"

class RendererBase;

class MeshComponent : public Component
{
public:
	MeshComponent(Object* _owner, RendererBase* _renderer, const char* meshID);
	~MeshComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	inline const char* GetMeshID() { return MeshID; }

private:
	bool bVisible;
	const char* MeshID;

	RendererBase* _Renderer;
};