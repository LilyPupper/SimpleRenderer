#pragma once

#include "Object.h"
#include "CharTexture.h"

#include <vector>

class Application
{
public:
	Application(const int& _width, const int& _height);
	~Application();

	void Run();

	inline CharTexture& GetRenderTexture() { return m_RenderTex; }

	static Application* INSTANCE;

private:
	CharTexture m_RenderTex;
	std::vector<Object*> m_Objects;
	bool m_Running = true;
	float m_FPS = 7.5f;
};
