#include "Application.h"

#include "Objects/RotatingModel.h"
#include "Components/TransformComponent.h"
#include "Renderer/ConsoleRenderer.h"

Application::Application(const int& _width, const int& _height)
	: m_Renderer(new ConsoleRenderer(_width, _height))
{
	m_Renderer->Initialise();

	// Add objects here
	RotatingModel* rm1 = new RotatingModel(m_Renderer, "models\\Torus_Low.obj");
	TransformComponent* t1 = static_cast<TransformComponent*>(rm1->FindComponentOfType(TRANSFORM));
	if (t1 != nullptr)
	{
		t1->SetScale(10.0f);
		t1->SetPosition(60.0f, 15.0f, 0.0f);
	}

	m_Objects.push_back(rm1);
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
	const clock_t start_time = clock();
	clock_t begin_frame = start_time;
	float deltaTime;

	while (m_Running)
	{
		m_Renderer->Flush();

		// Delta time
		deltaTime = float(clock() - begin_frame) / CLOCKS_PER_SEC;

		// Reseting the frame
		begin_frame = clock();

		for (unsigned int i = 0; i < m_Objects.size(); ++i)
		{
			m_Objects[i]->Update(deltaTime);
			m_Objects[i]->Render();
		}

		if (m_Renderer)
			m_Renderer->Render(deltaTime);
	}
}
