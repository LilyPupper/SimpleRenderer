#pragma once

#include "Component.h"

class Renderer;

class MeshComponent : public Component
{
public:
	MeshComponent(Object* _owner, Renderer* _renderer, const char* meshID);
	~MeshComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	inline const char* GetMeshID() { return MeshID; }

private:
	bool bVisible;
	const char* MeshID;

	Renderer* _Renderer;
};