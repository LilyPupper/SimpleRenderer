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

	//glm::vec3 forward = t->Get

	const float speed = 200.5f * _deltaTime;
	if (GetKeyState('W') & 0x8000)
	{
		glm::vec3 move = t->GetForward() * speed;
		t->Translate(move);
	}
	if (GetKeyState('S') & 0x8000)
	{
		glm::vec3 move = -t->GetForward() * speed;
		t->Translate(move);
	}
	if (GetKeyState('D') & 0x8000)
	{
		glm::vec3 move = t->GetRight() * speed;
		t->Translate(move);
	}
	if (GetKeyState('A') & 0x8000)
	{
		glm::vec3 move = -t->GetRight() * speed;
		t->Translate(move);
	}
	if (GetKeyState(' ') & 0x8000)
	{
		glm::vec3 move = t->GetUp() * speed;
		t->Translate(move);
	}
	//if (GetKeyState('') & 0x8000)
	//{
	//	glm::vec3 move = -t->GetUp() * speed;
	//	t->Translate(move);
	//}
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