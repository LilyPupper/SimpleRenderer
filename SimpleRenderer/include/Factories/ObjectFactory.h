#pragma once

#include "FactoryBase.h"
#include "Objects/Object.h"

class Renderer;

class ObjectFactory : public FactoryBase
{
public:
	ObjectFactory(Renderer* _renderer, std::vector<Object*>& _objectList);
	virtual ~ObjectFactory() override {}

	template<typename T>
	T* NewObject()
	{
		ObjectFactory::IsTypeDerivedFrom<T, Object>();
		return static_cast<T*>(ObjectFactory::NewObject(T::GetObjectType()));
	}

private:
	Object* NewObject(ObjectType type);

	Renderer* _Renderer;
	std::vector<Object*>& ObjectList;
};