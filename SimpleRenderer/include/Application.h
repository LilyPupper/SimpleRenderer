#pragma once

#include "Objects/Object.h"

#include <vector>

class Renderer;

class Application
{
public:
	Application(const int& _width, const int& _height);
	~Application();

	void Run();

private:
	std::vector<Object*> m_Objects;
	Renderer* m_Renderer;
	bool m_Running = true;
};
