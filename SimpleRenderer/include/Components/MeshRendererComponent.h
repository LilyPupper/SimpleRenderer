#pragma once

#include "Component.h"

class Renderer;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(Object* _owner, Renderer* _renderer);
	~MeshRendererComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

private:
	Renderer* m_Renderer;
};