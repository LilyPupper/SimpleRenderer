#include "Objects/Camera.h"

#include "Components/TransformComponent.h"
#include "Renderer/ConsoleRenderer.h"

#include <Windows.h>

Camera* Camera::Main;

Camera::Camera()
{
	if (Main)
	{
		assert("Too many cameras!", true);
	}

	Main = this;
}

Camera::~Camera()
{}

void Camera::Update(const float& _deltaTime)
{
	Object::Update(_deltaTime);

	TransformComponent* t = GetTransform();
	if (!t)
	{
		return;
	}

	const float speed = 1000.0f;
	if (GetKeyState('A') & 0x8000)
	{
		t->Translate(-_deltaTime * speed, 0.0f, 0.0f);
	}
	if (GetKeyState('D') & 0x8000)
	{
		t->Translate(_deltaTime * speed, 0.0f, 0.0f);
	}
}

void Camera::Render()
{
	Object::Render();
}

glm::mat4 Camera::GetCameraMatrix()
{
	if (!Main)
	{
		return glm::identity<glm::mat4>();
	}

	TransformComponent* t = Main->GetTransform();
	if (!t)
	{
		return glm::identity<glm::mat4>();
	}

	return t->GetTransformation();
}