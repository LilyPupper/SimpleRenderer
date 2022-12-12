#pragma once

#include "Object.h"
#include "CharTexture.h"

#include <Windows.h>
#include <string>
#include <vector>

class Application
{
public:
	Application(const int& _width, const int& _height);
	~Application();

	void WriteToScreen(int _row, int _col, wchar_t _char);
	void WriteToScreen(int _row, int _col, const std::wstring& _s);

	void Run();

private:
	CharTexture m_RenderTex;
	std::vector<Object*> m_Objects;
	bool m_Running = true;
	float m_FPS = 1000.f;

	wchar_t* m_ScreenBuffer;
	HANDLE m_Console;
};
