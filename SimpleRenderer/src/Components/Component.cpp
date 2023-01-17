#include "Components/Component.h"

Component::Component(Object* _owner)
	: m_Owner(_owner)
{}

Component::~Component()
{}

COMPONENT_TYPE Component::GetComponentType()
{
	return m_Type;
}

Object* Component::GetOwner()
{
	return m_Owner;
}
