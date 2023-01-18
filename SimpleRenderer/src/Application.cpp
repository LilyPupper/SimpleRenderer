#include "Application.h"

#include "Objects/RotatingModel.h"
#include "Components/TransformComponent.h"
#include "Renderer/ConsoleRenderer.h"

#include <chrono>

#define CURRENT_TIME_MS std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count()

Application::Application(const int& _width, const int& _height)
	: m_Renderer(new ConsoleRenderer(_width, _height))
{
	m_Renderer->Initialise();

	// Add objects here
	//RotatingModel* rm1 = new RotatingModel(m_Renderer, "models\\Triangle.obj");
	//TransformComponent* t1 = static_cast<TransformComponent*>(rm1->FindComponentOfType(TRANSFORM));
	//if (t1 != nullptr)
	//{
	//	t1->SetScale(5.0f);
	//	t1->SetPosition(20.0f, 15.0f, 0.0f);
	//}
	//
	//RotatingModel* rm2 = new RotatingModel(m_Renderer, "models\\Cube.obj");
	//TransformComponent* t2 = static_cast<TransformComponent*>(rm2->FindComponentOfType(TRANSFORM));
	//if (t2 != nullptr)
	//{
	//	t2->SetScale(10.0f);
	//	t2->SetPosition(60.0f, 15.0f, 50.0f);
	//}
	
	RotatingModel* rm3 = new RotatingModel(m_Renderer, "models\\Torus_Low.obj");
	TransformComponent* t3 = static_cast<TransformComponent*>(rm3->FindComponentOfType(TRANSFORM));
	if (t3 != nullptr)
	{
		t3->SetScale(10.0f);
		t3->SetPosition(100.0f, 15.0f, 0.0f);
	}

	//m_Objects.push_back(rm1);
	//m_Objects.push_back(rm2);
	m_Objects.push_back(rm3);
}

Application::~Application()
{
	for (unsigned int i = 0; i < m_Objects.size(); ++i)
	{
		delete m_Objects[i];
	}

	delete m_Renderer;
}

void Application::Run()
{
	// Time
	auto begin_frame = CURRENT_TIME_MS;
	double deltaTime;

	while (m_Running)
	{
		// Delta time
		deltaTime = (CURRENT_TIME_MS - begin_frame) * 0.001;

		// Reseting the frame
		begin_frame = CURRENT_TIME_MS;

		if (m_Renderer)
			m_Renderer->Flush(deltaTime);

		for (unsigned int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(deltaTime);
			m_Objects[i]->Render();
		}
	}
}
