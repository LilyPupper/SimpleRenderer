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
	std::vector<Object*> m_ObjectList;
	RendererBase* m_Renderer;
	ObjectFactory* m_ObjectFactory;

	bool m_Running = true;
};
