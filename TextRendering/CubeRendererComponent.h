#pragma once

#include "Component.h"

#include "Mat3.h"
#include "Vec3.h"

class CubeRendererComponent : public Component
{
public:
	CubeRendererComponent(Object* _owner);
	~CubeRendererComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

private:
	// Rendering
	Mat3 projection = Mat3::Identity();

	// Rotations
	float angleX = 0.f;
	float angleY = 0.f;
	float angleZ = 0.f;

	float angleXIncrease = 1.5f;
	float angleYIncrease = 0.75f;
	float angleZIncrease = 0.5f;

	float scale = 1.f;

	float scaleIncrease = 0.f;

	// 3D Objects to be rendered
	Vec3* cube[8];
};