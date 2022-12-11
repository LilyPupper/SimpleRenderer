#pragma once

#include <vector>
#include <map>

class Object;
class Component;
enum COMPONENT_TYPE;

typedef std::map<const unsigned int, Object*> OBJECT_MAP;

class Object
{
public:
	Object();
	~Object();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	void AddComponent(Component* _component);
	Component* FindComponentOfType(COMPONENT_TYPE _type);
	Component* operator[](COMPONENT_TYPE _type);

	unsigned int GetObjectID();

	static OBJECT_MAP GetObjectList();

private:
	unsigned int m_ObjectID;
	std::vector<Component*> m_Components;

	static unsigned int s_ObjectIDCount;
	static OBJECT_MAP s_ObjectList;
};

