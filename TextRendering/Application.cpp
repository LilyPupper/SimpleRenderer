#include "Application.h"

#include "Component.h"
#include "RotatingCube.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <conio.h>

Application::Application(const int& _width, const int& _height)
	: m_RenderTex(_width, _height)
{
	// Set console size
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect = { 0, 0, _width, _height };
	SetConsoleWindowInfo(hConsole, TRUE, &rect);

	// Create Screen Buffer
	m_ScreenBuffer = new wchar_t[_width * _height];
	std::fill_n(m_ScreenBuffer, _width * _height, L'a');

	m_Console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (m_Console == INVALID_HANDLE_VALUE)
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		m_Running = false;
	}
	if (!SetConsoleActiveScreenBuffer(m_Console))
	{
		std::cerr << "CreateConsoleScreenBuffer win error: " << GetLastError() << '\n';
		m_Running = false;
	}

	// Add objects here
	m_Objects.push_back(new RotatingCube());
}

Application::~Application()
{
	for (unsigned int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
	}

	delete m_ScreenBuffer;
}

void Application::WriteToScreen(int _row, int _col, wchar_t _char)
{
	m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col] = _char;
}

void Application::WriteToScreen(int _row, int _col, const std::wstring& _s)
{
	swprintf(&m_ScreenBuffer[_row * m_RenderTex.GetWidth() + _col], _s.size() + 1, L"%s", _s.c_str());
}

void Application::Run()
{
	// Time
	const clock_t start_time = clock();
	clock_t begin_frame = start_time;
	clock_t end_frame;
	float deltaTime;
	const float frameTime = 1.0f / m_FPS;

	// Border string
	std::wstring border;
	for (unsigned int i = 0; i < m_RenderTex.GetWidth(); ++i)
		border += L"-";

	while (m_Running)
	{
		DWORD dwBytesWritten = 0;

		// Delta time
		deltaTime = float(clock() - begin_frame) / CLOCKS_PER_SEC;

		// Reseting the frame
		begin_frame = clock();

		// Clearing the screen
		m_RenderTex.Clear();

		for (unsigned int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(deltaTime);
			m_Objects[i]->Render(&m_RenderTex);
		}

		// Drawing the char map
		for (unsigned int y = 0; y < m_RenderTex.GetWidth(); ++y)
		{
			for (unsigned int x = 0; x < m_RenderTex.GetHeight(); ++x)
			{
				COORD c;
				c.X = y;
				c.Y = x;
				if (m_RenderTex[y][x].Data == 0)
				{
					WriteToScreen(x, y, L' ');
				}
				else if (m_RenderTex[y][x].Data == 1)
				{
					WORD attr = m_RenderTex[y][x].Color;
					WriteConsoleOutputAttribute(m_Console, &attr, 1, c, &dwBytesWritten);
					WriteToScreen(x, y, L'#');
				}
			}
		}

		WriteToScreen(0, 0, border);
		WriteToScreen(m_RenderTex.GetHeight() - 1, 0, L"FPS: " + std::to_wstring(1.0f / deltaTime));
		
		WriteConsoleOutputCharacter(m_Console, m_ScreenBuffer, m_RenderTex.GetWidth() * m_RenderTex.GetHeight(), { 0,0 }, &dwBytesWritten);

		// Waiting for the end of the frame to keep the fps
		if (m_VSync)
			while ((float(clock() - begin_frame)) / CLOCKS_PER_SEC < frameTime);
	}
}
