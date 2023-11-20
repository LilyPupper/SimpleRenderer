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

	// Move
	const float moveSpeed = 10.f * _deltaTime;
	if (GetKeyState('W') & 0x8000)
	{
		const glm::vec3 move = -t->GetForward() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('S') & 0x8000)
	{
		const glm::vec3 move = t->GetForward() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('D') & 0x8000)
	{
		const glm::vec3 move = -t->GetRight() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('A') & 0x8000)
	{
		const glm::vec3 move = t->GetRight() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('Q') & 0x8000)
	{
		const glm::vec3 move = t->GetUp() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('E') & 0x8000)
	{
		const glm::vec3 move = -t->GetUp() * moveSpeed;
		t->Translate(move);
	}

	// Rotate
	const float rotSpeed = 50.f * _deltaTime;
	if (GetKeyState(VK_UP) & 0x8000)
	{
		const float rot = -rotSpeed;
		t->RotateX(rot);
	}
	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		const float rot = rotSpeed;
		t->RotateX(rot);
	}
	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		const float rot = -rotSpeed;
		t->RotateY(rot);
	}
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		const float rot = rotSpeed;
		t->RotateY(rot);
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