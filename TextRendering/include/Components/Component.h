#pragma once

#include "Objects/Object.h"

class CharTexture;
class TransformComponent;
enum COMPONENT_TYPE
{
	TRANSFORM,
	MESH,
	MESHRENDERER,
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

	inline const bool& IsActive() { return m_Active; }
	inline void SetActive(const bool& _active) { m_Active = _active; }

protected:
	COMPONENT_TYPE m_Type;
	Object* m_Owner;

	bool m_Active = true;
};