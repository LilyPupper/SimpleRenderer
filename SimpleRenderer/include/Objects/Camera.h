#pragma once

#include "glm/fwd.hpp"

#include "Object.h"

class Camera : public Object
{
public:
	Camera();
	~Camera();

	static ObjectType GetObjectType() { return ObjectType::MovableCamera; }

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	static glm::mat4 GetCameraMatrix();

	static Camera* Main;

private:
};