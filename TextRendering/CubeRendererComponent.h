#pragma once

#include "Component.h"

#include "Mat3.h"
#include "Vec3.h"
#include "Tri.h"

#include <vector>
#include <future>

class CharTexture;

class CubeRendererComponent : public Component
{
public:
	CubeRendererComponent(Object* _owner);
	~CubeRendererComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

	void Render_Single(CharTexture* _texture);
	void Render_Async(CharTexture* _texture);
	void RenderThread(CharTexture* _texture, unsigned int _rowStart, unsigned int _rowEnd);

private:
	Mat3 m_Projection;
	Vec3 m_RotAngles;
	float m_Scale;

	float m_Time;

	// 3D Objects to be rendered
	std::vector<Vec3> m_Vertices;
	std::vector<Tri> m_Triangles;

	// Async
	std::vector<std::future<void>> m_Futures;
};