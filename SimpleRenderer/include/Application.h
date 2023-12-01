#pragma once

#include "Objects/Object.h"

#include <vector>

class RendererBase;
class ObjectFactory;

class Application
{
public:
	Application(const int& _width, const int& _height);
	~Application();

	void Run();

private:
	std::vector<Object*> ObjectList;
	RendererBase* Renderer;
	ObjectFactory* _ObjectFactory;

	bool Running;
};
