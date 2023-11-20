#include "Application.h"

#include "Objects/Camera.h"
#include "Objects/RotatingModel.h"
#include "Components/TransformComponent.h"
#include "Renderer/ConsoleRenderer.h"
#include "Renderer/SDLRenderer.h"
#include "Factories/ObjectFactory.h"

#include <chrono>

#define CURRENT_TIME_MS std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count()

Application::Application(const int& _width, const int& _height)
	: m_Renderer(new ConsoleRenderer(_width, _height))
	, m_ObjectFactory(new ObjectFactory(m_Renderer, m_ObjectList))
	, m_Running(true)
	, m_CursorLocked(false)
{
	Camera* cam = m_ObjectFactory->NewObject<Camera>();
	if (TransformComponent* tc = cam->GetTransform())
	{
		tc->SetPosition(0.f, 0.f, 0.f);
	}

	float range = 15.f;
	float step = 15.f;
	for (float x = -range; x < range; x += step)
	{
		for (float y = -range; y < range; y += step)
		{
			for (float z = -range; z < range; z += step)
			{
				RotatingModel* rm1 = m_ObjectFactory->NewObject<RotatingModel>();
				if (TransformComponent* t1 = rm1->GetTransform())
				{
					t1->SetScale(5.f);
					t1->SetPosition(x, y, z);
					t1->SetRotation(glm::vec3(0.f, 0.f, 0.f));
				}
			}
		}
	}

	//RotatingModel* rm1 = m_ObjectFactory->NewObject<RotatingModel>();
	//if (TransformComponent* t1 = rm1->GetTransform())
	//{
	//	t1->SetScale(5.f);
	//	t1->SetPosition(0.f, 0.0f, 1.0f);
	//	t1->SetRotation(glm::vec3(0.f, 0.f, 0.f));
	//}
}

Application::~Application()
{
	for (unsigned int i = 0; i < m_ObjectList.size(); ++i)
	{
		delete m_ObjectList[i];
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

		//ProcessInput();

		for (unsigned int i = 0; i < m_ObjectList.size(); ++i)
		{
			m_ObjectList[i]->Update(deltaTimeF);
			m_ObjectList[i]->Render();
		}

		if (m_Renderer)
		{
			m_Renderer->Render(deltaTime);
		}
	}
}

void Application::ProcessInput()
{
	if (GetKeyState(VK_ESCAPE) & 0x8000)
	{
		m_Running = false;
	}

	ClipMouse();
}

void Application::ClipMouse()
{
	if (HWND window = m_Renderer->GetWindow())
	{
		RECT rect;
		GetClientRect(m_Renderer->GetWindow(), &rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints(window, nullptr, &ul, 1);
		MapWindowPoints(window, nullptr, &lr, 1);

		rect.left = ul.x;
		rect.top = ul.y;

		rect.right = lr.x;
		rect.bottom = lr.y;

		ClipCursor(&rect);
	}
}