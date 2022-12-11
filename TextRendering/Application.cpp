#include "Application.h"

#include "Component.h"
#include "RotatingCube.h"

#include <time.h>
#include <iostream>
#include <cassert>

Application* Application::INSTANCE;

Application::Application(const int& _width, const int& _height)
	: m_RenderTex(_width, _height)
{
	assert(!INSTANCE && "Application::INSTANCE already exists!");

	INSTANCE = this;

	// Add objects here
	m_Objects.push_back(new RotatingCube());
}

Application::~Application()
{
	for (unsigned int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
	}
}

void Application::Run()
{
	// Time
	const clock_t start_time = clock();
	clock_t begin_frame;
	float deltaTime;
	const float frameTime = 1.0f / m_FPS;
	clock_t last_time = clock();

	while (m_Running)
	{
		// Reseting the frame
		begin_frame = clock();

		// Delta time
		deltaTime = float(clock() - last_time) / CLOCKS_PER_SEC;

		// Clearing the screen
		system("cls");
		m_RenderTex.Clear();

		for (unsigned int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(deltaTime);
			m_Objects[i]->Render();
		}

		// Drawing the char map
		for (unsigned int y = 0; y < m_RenderTex.GetHeight(); ++y)
		{
			for (unsigned int x = 0; x < m_RenderTex.GetWidth(); ++x)
			{
				if (m_RenderTex[y][x] == 0)
				{
					std::cout << " ";
				}
				else
				{
					std::cout << "0";
				}
			}

			std::cout << std::endl;
		}

		// Calculating timing
		std::cout << "Time: " << float(clock() - start_time) / CLOCKS_PER_SEC << std::endl;

		std::cout << "DeltaTime: " << deltaTime << std::endl;
		last_time = float(clock());

		// Waiting for the end of the frame to keep the fps
		while ((float(clock() - begin_frame)) / CLOCKS_PER_SEC < frameTime);
	}
}