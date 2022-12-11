#pragma once

#include "Object.h"

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
	virtual void Render() = 0;

	COMPONENT_TYPE GetComponentType();
	Object* GetOwner();

protected:
	COMPONENT_TYPE m_Type;
	Object* m_Owner;
};