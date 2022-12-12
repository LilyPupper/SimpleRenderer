#pragma once

#include "Component.h"

#include "Vec3.h"

class TransformComponent : public Component
{
public:
	TransformComponent(Object* _owner);
	~TransformComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

	Vec3 m_Position;
};