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
	RotatingModel* rm1 = new RotatingModel(m_Renderer, "models\\Cross.obj");
	TransformComponent* t1 = static_cast<TransformComponent*>(rm1->FindComponentOfType(TRANSFORM));
	if (t1 != nullptr)
	{
		t1->SetScale(2.5f);
		t1->SetPosition(10.0f, 15.0f, 0.0f);
		t1->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
	}
	m_Objects.push_back(rm1);
	
	RotatingModel* rm2 = new RotatingModel(m_Renderer, "models\\Cube.obj");
	TransformComponent* t2 = static_cast<TransformComponent*>(rm2->FindComponentOfType(TRANSFORM));
	if (t2 != nullptr)
	{
		t2->SetScale(5.0f);
		t2->SetPosition(30.0f, 15.0f, 0.0f);
		t2->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
	}
	m_Objects.push_back(rm2);
	
	RotatingModel* rm3 = new RotatingModel(m_Renderer, "models\\Triangle.obj");
	TransformComponent* t3 = static_cast<TransformComponent*>(rm3->FindComponentOfType(TRANSFORM));
	if (t3 != nullptr)
	{
		t3->SetScale(2.5f);
		t3->SetPosition(45.0f, 15.0f, 0.0f);
		t3->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
	}
	m_Objects.push_back(rm3);
	
	RotatingModel* rm4 = new RotatingModel(m_Renderer, "models\\Torus_Low.obj");
	TransformComponent* t4 = static_cast<TransformComponent*>(rm4->FindComponentOfType(TRANSFORM));
	if (t4 != nullptr)
	{
		t4->SetScale(5.0f);
		t4->SetPosition(60.0f, 15.0f, 0.0f);
		t4->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
	}
	m_Objects.push_back(rm4);
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
	float deltaTimeF;

	while (m_Running)
	{
		// Delta time
		deltaTime = (CURRENT_TIME_MS - begin_frame) * 0.001;
		deltaTimeF = static_cast<float>(deltaTime);

		// Reseting the frame
		begin_frame = CURRENT_TIME_MS;

		if (m_Renderer)
			m_Renderer->Flush(deltaTimeF);

		for (unsigned int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(deltaTimeF);
			m_Objects[i]->Render();
		}
	}
}
