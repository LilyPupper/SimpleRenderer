#include "Objects/Camera.h"

#include "Components/TransformComponent.h"
#include "Renderer/ConsoleRenderer.h"

#include <Windows.h>

Camera* Camera::Main;

Camera::Camera(const int _pixelWidth /*= 0*/, const int _pixelHeight /*= 0*/, const int _viewportX /*= 0*/, const int _viewportY /*= 0*/, const float _FOV /*= 60.f*/, const float _nearPlane /*= 0.1f*/, const float _farPlane /*= 1000.f*/)
	: PixelWidth(200)
	, PixelHeight(200)
	, ViewportX(_viewportX)
	, ViewportY(_viewportY)
	, FOV(_FOV)
	, AspectRatio((float)_pixelWidth/(float)_pixelHeight)
	, NearPlane(_nearPlane)
	, FarPlane(_farPlane)
{
	if (Main)
	{
		assert("Too many cameras!" & 1);
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
		const glm::vec3 move = t->GetRight() * moveSpeed;
		t->Translate(move);
	}
	if (GetKeyState('A') & 0x8000)
	{
		const glm::vec3 move = -t->GetRight() * moveSpeed;
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

	// Move viewport
	const float viewportSpeed = 25.f * _deltaTime;
	if (GetKeyState('I') & 0x8000)
	{
		ViewportY -= viewportSpeed;
	}
	if (GetKeyState('K') & 0x8000)
	{
		ViewportY += viewportSpeed;
	}
	if (GetKeyState('J') & 0x8000)
	{
		ViewportX -= viewportSpeed;
	}
	if (GetKeyState('L') & 0x8000)
	{
		ViewportX += viewportSpeed;
	}
}

void Camera::Render()
{
	Object::Render();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::inverse(GetTransform()->GetTransformation());
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(glm::radians(FOV), AspectRatio, NearPlane, FarPlane);
}

glm::mat4 Camera::GetMVPMatrix(TransformComponent* _transform)
{
	const glm::mat4 view = GetViewMatrix();
	const glm::mat4 model = _transform->GetTransformation();
	const glm::mat4 projection = GetProjectionMatrix();
	const glm::mat4 mvp = projection * view * model;
	return mvp;
}

glm::mat4 Camera::GetMV(TransformComponent* _transform)
{
	const glm::mat4 view = GetViewMatrix();
	const glm::mat4 model = _transform->GetTransformation();
	const glm::mat4 mv = view * model;
	return mv;
}

Tri Camera::TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform)
{
	glm::mat4 MVP = GetMVPMatrix(_transform);

	glm::vec4 pos1 = MVP * _tri.v1;
	glm::vec4 pos2 = MVP * _tri.v2;
	glm::vec4 pos3 = MVP * _tri.v3;
	
	Tri transformedTri(pos1, pos2, pos3);
	transformedTri.RecalculateSurfaceNormal();

	// Backface culling
	// as we're in clip space, avgPos is a direction vector from the camera to the centre of the tri
	glm::vec3 avgPos = (pos1 + pos2 + pos3) / 3.f;
	if (glm::dot(transformedTri.GetSurfaceNormal(), avgPos) >= 0.f)
	{
		transformedTri.Discard = true;
		return transformedTri;
	}

	pos1.x /= pos1.w;
	pos2.x /= pos2.w;
	pos3.x /= pos3.w;
	
	pos1.y /= pos1.w;
	pos2.y /= pos2.w;
	pos3.y /= pos3.w;
	
	pos1.z /= pos1.w;
	pos2.z /= pos2.w;
	pos3.z /= pos3.w;

	pos1.z = (((FarPlane - NearPlane) * pos1.z) + NearPlane + FarPlane) / 2.f;
	pos2.z = (((FarPlane - NearPlane) * pos2.z) + NearPlane + FarPlane) / 2.f;
	pos3.z = (((FarPlane - NearPlane) * pos3.z) + NearPlane + FarPlane) / 2.f;

	transformedTri.v1.x = ((pos1.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;
	transformedTri.v2.x = ((pos2.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;
	transformedTri.v3.x = ((pos3.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;
	
	transformedTri.v1.y = ((pos1.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
	transformedTri.v2.y = ((pos2.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
	transformedTri.v3.y = ((pos3.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
						  
	// Frustum culling
	auto OutsideFrustum = [this](const Tri& _tri) -> bool
	{
		auto ClipVec3 = [this](const glm::vec4& _vec) -> bool
		{
			return  (_vec.x < ViewportX) || (_vec.x > (float)PixelWidth) ||
					(_vec.y < ViewportY) || (_vec.y > (float)PixelHeight) ||
					(_vec.z < NearPlane) || (_vec.z > FarPlane);			
		};
	
		return ClipVec3(_tri.v1) && ClipVec3(_tri.v2) && ClipVec3(_tri.v3);
	};
	
	if (OutsideFrustum(transformedTri))
	{
		transformedTri.Discard = true;
	}

	return transformedTri;
}