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
	: Renderer(new ConsoleRenderer(_width, _height))
	, _ObjectFactory(new ObjectFactory(Renderer, ObjectList))
	, Running(true)
{
	Camera* cam = _ObjectFactory->NewObject<Camera>();
	cam->PixelWidth = _width;
	cam->PixelHeight = _height;
	cam->ViewportX = 0;
	if (TransformComponent* tc = cam->GetTransform())
	{
		tc->SetPosition(0.f, 0.f, 0.f);
		tc->SetRotation({0.0f, 0.0f, 0.0f});
	}

	RotatingModel* skull = _ObjectFactory->NewObject<RotatingModel>({nullptr, "models/Skull.obj"});
	skull->SetActive(false);
	if (TransformComponent* skullTransform = skull->GetTransform())
	{
		skullTransform->SetScale(1.5f);
		skullTransform->SetPosition(0.f, 0.f, -50.f);
		skullTransform->SetRotation(glm::vec3(90.f, 180.f, 0.f));
	}
	
	RotatingModel* cat = _ObjectFactory->NewObject<RotatingModel>({nullptr, "models/Cat.obj"});
	cat->SetActive(false);
	if (TransformComponent* catTransform = cat->GetTransform())
	{
		catTransform->SetScale(10.f);
		catTransform->SetPosition(50.f, 0.f, -50.f);
		catTransform->SetRotation(glm::vec3(180.f, -90.f, 0.f));
	}

	RotatingModel* monkey = _ObjectFactory->NewObject<RotatingModel>({nullptr,"models/Monkey.obj"});
	monkey->SetActive(false);
	if (TransformComponent* monkeyTransform = monkey->GetTransform())
	{
		monkeyTransform->SetScale(20.f);
		monkeyTransform->SetPosition(-50.f, -20.f, -50.f);
		monkeyTransform->SetRotation(glm::vec3(180.f, 180.f, 0.f));
	}
	
	RotatingModel* triangle = _ObjectFactory->NewObject<RotatingModel>({ nullptr,"models/Triangle.obj" });
	if (TransformComponent* triangleTransform = triangle->GetTransform())
	{
		triangleTransform->SetScale(4.f);
		triangleTransform->SetPosition(50.f, -20.f, 50.f);
		triangleTransform->SetRotation(glm::vec3(0.f, 0.f, 0.f));
	}
	
	RotatingModel* torusHigh = _ObjectFactory->NewObject<RotatingModel>({nullptr, "models/Torus_High.obj" });
	if (TransformComponent* torusTransform = torusHigh->GetTransform())
	{
		torusTransform->SetScale(15.f);
		torusTransform->SetPosition(0.f, -20.f, 50.f);
		torusTransform->SetRotation(glm::vec3(0.f, 0.f, 0.f));
	}
	
	RotatingModel* cube = _ObjectFactory->NewObject<RotatingModel>({nullptr, "models/Cube.obj" });
	if (TransformComponent* cubeTransform = cube->GetTransform())
	{
		cubeTransform->SetScale(5.f);
		cubeTransform->SetPosition(-50.f, -20.f, 50.f);
		cubeTransform->SetRotation(glm::vec3(0.f, 0.f, 0.f));
	}
	
	RotatingModel* cube1 = _ObjectFactory->NewObject<RotatingModel>({cube, "models/Cube.obj" });
	cube1->SetActive(false);
	if (TransformComponent* cube1Transform = cube1->GetTransform())
	{
		cube1Transform->SetScale(1.f);
		cube1Transform->SetPosition(-10.f, 0.f, 0.f);
		cube1Transform->SetRotation(glm::vec3(0.f, 0.f, 0.f));
	}
}

Application::~Application()
{
	for (unsigned int i = 0; i < ObjectList.size(); ++i)
	{
		delete ObjectList[i];
	}

	delete Renderer;
}

void Application::Run()
{
	// Time
	auto begin_frame = CURRENT_TIME_MS;
	double deltaTime;
	float deltaTimeF;

	while (Running)
	{
		// Delta time
		deltaTime = (CURRENT_TIME_MS - begin_frame) * 0.001;
		deltaTimeF = static_cast<float>(deltaTime);

		// Reseting the frame
		begin_frame = CURRENT_TIME_MS;

		for (unsigned int i = 0; i < ObjectList.size(); ++i)
		{
			if (ObjectList[i]->IsActive())
			{
				ObjectList[i]->Update(deltaTimeF);
			}
			ObjectList[i]->Render();
		}

		if (Renderer)
		{
			Renderer->Render(deltaTime);
		}
	}
}