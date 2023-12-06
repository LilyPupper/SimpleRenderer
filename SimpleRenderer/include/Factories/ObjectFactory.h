#pragma once

#include "FactoryBase.h"
#include "Objects/Object.h"

#include <string>

class RendererBase;

struct ObjectDefinition
{
	Object* Parent;
	std::string ModelPath;
};

class ObjectFactory : public FactoryBase
{
public:
	ObjectFactory(RendererBase* _renderer, std::vector<Object*>& _objectList);
	virtual ~ObjectFactory() override {}

	template<typename T>
	T* NewObject(const ObjectDefinition& _definition = {})
	{
		ObjectFactory::IsTypeDerivedFrom<T, Object>();
		return static_cast<T*>(ObjectFactory::NewObject(T::GetObjectType(), _definition));
	}

private:
	Object* NewObject(ObjectType type, const ObjectDefinition& _definition);

	RendererBase* _Renderer;
	std::vector<Object*>& ObjectList;
};