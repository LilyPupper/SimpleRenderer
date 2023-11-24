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
		assert("Too many cameras!" && 1);
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

glm::mat4 Camera::GetMVMatrix(TransformComponent* _transform)
{
	const glm::mat4 view = GetViewMatrix();
	const glm::mat4 model = _transform->GetTransformation();
	const glm::mat4 mv = view * model;
	return mv;
}

Tri Camera::TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform)
{
	glm::mat4 MVP = GetMVPMatrix(_transform);
	Vertex vert1(MVP * _tri.v1.Position, _tri.v1.Normal);
	Vertex vert2(MVP * _tri.v2.Position, _tri.v2.Normal);
	Vertex vert3(MVP * _tri.v3.Position, _tri.v3.Normal);
	
	Tri transformedTri(vert1, vert2, vert3);
	transformedTri.RecalculateSurfaceNormal();

	// Backface culling
	// as we're in clip space, avgPos is a direction vector from the camera to the centre of the tri
	glm::vec3 avgPos = (vert1.Position + vert2.Position + vert3.Position) / 3.f;
	if (glm::dot(glm::vec3(transformedTri.GetSurfaceNormal()), avgPos) >= 0.f)
	{
		transformedTri.Discard = true;
		return transformedTri;
	}

	vert1.Position.x /= vert1.Position.w;
	vert2.Position.x /= vert2.Position.w;
	vert3.Position.x /= vert3.Position.w;

	vert1.Position.y /= vert1.Position.w;
	vert2.Position.y /= vert2.Position.w;
	vert3.Position.y /= vert3.Position.w;

	vert1.Position.z /= vert1.Position.w;
	vert2.Position.z /= vert2.Position.w;
	vert3.Position.z /= vert3.Position.w;

	vert1.Position.z = (((FarPlane - NearPlane) * vert1.Position.z) + NearPlane + FarPlane) / 2.f;
	vert2.Position.z = (((FarPlane - NearPlane) * vert2.Position.z) + NearPlane + FarPlane) / 2.f;
	vert3.Position.z = (((FarPlane - NearPlane) * vert3.Position.z) + NearPlane + FarPlane) / 2.f;

	transformedTri.v1.Position.x = ((vert1.Position.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;
	transformedTri.v2.Position.x = ((vert2.Position.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;
	transformedTri.v3.Position.x = ((vert3.Position.x + 1.0f) * ((float)PixelWidth / 2)) + ViewportX;

	transformedTri.v1.Position.y = ((vert1.Position.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
	transformedTri.v2.Position.y = ((vert2.Position.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
	transformedTri.v3.Position.y = ((vert3.Position.y + 1.0f) * ((float)PixelHeight / 2)) + ViewportY;
						  
	// Frustum culling
	auto OutsideFrustum = [this](const Tri& _tri) -> bool
	{
		auto ClipVec3 = [this](const glm::vec4& _vec) -> bool
		{
			return  (_vec.x < ViewportX) || (_vec.x > (float)PixelWidth) ||
					(_vec.y < ViewportY) || (_vec.y > (float)PixelHeight) ||
					(_vec.z < NearPlane) || (_vec.z > FarPlane);			
		};
	
		return ClipVec3(_tri.v1.Position) && ClipVec3(_tri.v2.Position) && ClipVec3(_tri.v3.Position);
	};
	
	if (OutsideFrustum(transformedTri))
	{
		transformedTri.Discard = true;
	}

	return transformedTri;
}