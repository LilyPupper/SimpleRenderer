#pragma once

#include "Objects/Object.h"

#include <vector>

class Renderer;
class ObjectFactory;

class Application
{
public:
	Application(const int& _width, const int& _height);
	~Application();

	void Run();

private:
	std::vector<Object*> m_ObjectList;
	Renderer* m_Renderer;
	ObjectFactory* m_ObjectFactory;

	bool m_Running = true;
};
