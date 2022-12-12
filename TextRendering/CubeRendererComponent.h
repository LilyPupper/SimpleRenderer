#pragma once

#include "Component.h"

#include "Mat3.h"
#include "Vec3.h"
#include "Tri.h"

#include <vector>

class CharTexture;

class CubeRendererComponent : public Component
{
public:
	CubeRendererComponent(Object* _owner);
	~CubeRendererComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

private:
	Mat3 m_Projection;
	Vec3 m_RotAngles;
	float m_Scale;

	float m_Time;

	// 3D Objects to be rendered
	std::vector<Vec3> m_Vertices;
	std::vector<Tri> m_Triangles;
};