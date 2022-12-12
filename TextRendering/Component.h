#pragma once

#include "Object.h"

class CharTexture;
class TransformComponent;
enum COMPONENT_TYPE
{
	TRANSFORM,
};

class Component
{
public:
	Component(Object* _owner);
	~Component();

	virtual void Update(const float& _deltaTime) = 0;
	virtual void Render(CharTexture* _texture) = 0;

	COMPONENT_TYPE GetComponentType();
	Object* GetOwner();
	inline TransformComponent* GetTransform() { return m_Owner->GetTransform(); }

protected:
	COMPONENT_TYPE m_Type;
	Object* m_Owner;
};