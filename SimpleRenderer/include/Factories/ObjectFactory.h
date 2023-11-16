#pragma once

#include "FactoryBase.h"
#include "Objects/Object.h"

class RendererBase;

class ObjectFactory : public FactoryBase
{
public:
	ObjectFactory(RendererBase* _renderer, std::vector<Object*>& _objectList);
	virtual ~ObjectFactory() override {}

	template<typename T>
	T* NewObject()
	{
		ObjectFactory::IsTypeDerivedFrom<T, Object>();
		return static_cast<T*>(ObjectFactory::NewObject(T::GetObjectType()));
	}

private:
	Object* NewObject(ObjectType type);

	RendererBase* _Renderer;
	std::vector<Object*>& ObjectList;
};