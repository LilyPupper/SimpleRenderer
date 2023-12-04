#pragma once

#include "glm/fwd.hpp"

#include "Object.h"
#include "Tri.h"

class Camera : public Object
{
public:
	Camera(const int _pixelWidth = 1, const int _pixelHeight = 1, const int _viewportX = 0, const int _viewportY = 0, const float _FOV = 60.f, const float _nearPlane = 1.f, const float _farPlane = 1000.f);
	~Camera();

	static ObjectType GetObjectType() { return ObjectType::MovableCamera; }

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	glm::mat4 GetCameraMatrix();

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix() const;
	glm::mat4 GetMVPMatrix(TransformComponent* _transform);
	// Get Model View matrix
	glm::mat4 GetMVMatrix(TransformComponent* _transform);

	Tri TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform);

	static Camera* Main;

	// viewport pixel dimensions
	int PixelWidth, PixelHeight;
	// viewport screen position from the top-left
	float ViewportX, ViewportY;

	// Frustum info
	float FOV;
	float AspectRatio;
	float NearPlane, FarPlane;

private:
	glm::vec3 Rotation;
};
