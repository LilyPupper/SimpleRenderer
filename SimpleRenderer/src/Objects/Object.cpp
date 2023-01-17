#include "Objects/Object.h"

#include "Components/Component.h"
#include "Components/TransformComponent.h"

unsigned int Object::s_ObjectIDCount = 0;
OBJECT_MAP Object::s_ObjectList;

Object::Object()
	: m_Transform(new TransformComponent(this))
{
	m_ObjectID = s_ObjectIDCount;
	++s_ObjectIDCount;

	s_ObjectList.insert(std::pair<const unsigned int, Object*>(m_ObjectID, this));

	AddComponent(m_Transform);
}

Object::~Object()
{}

void Object::Update(const float& _deltaTime)
{
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->Update(_deltaTime);
	}
}

void Object::Render()
{
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		m_Components[i]->Render();
	}
}

void Object::AddComponent(Component* _component)
{
	m_Components.push_back(_component);
}

Component* Object::FindComponentOfType(COMPONENT_TYPE _type)
{
	for (unsigned int i = 0; i < m_Components.size(); ++i)
	{
		if (m_Components[i]->GetComponentType() == _type)
			return m_Components[i];
	}
	return nullptr;
}

Component* Object::operator[](COMPONENT_TYPE _type)
{
	return FindComponentOfType(_type);
}

unsigned int Object::GetObjectID()
{
	return m_ObjectID;
}

OBJECT_MAP Object::GetObjectList()
{
	return s_ObjectList;
}
