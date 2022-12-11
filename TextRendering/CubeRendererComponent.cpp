#include "CubeRendererComponent.h"

#include "Application.h"
#include "CharTexture.h"

CubeRendererComponent::CubeRendererComponent(Object* _owner)
	: Component(_owner)
{
	cube[0] = new Vec3(-5.f, -5.f, 5.f);
	cube[1] = new Vec3(5.f, -5.f, 5.f);
	cube[2] = new Vec3(5.f, 5.f, 5.f);
	cube[3] = new Vec3(-5.f, 5.f, 5.f);

	cube[4] = new Vec3(-5.f, -5.f, -5.f);
	cube[5] = new Vec3(5.f, -5.f, -5.f);
	cube[6] = new Vec3(5.f, 5.f, -5.f);
	cube[7] = new Vec3(-5.f, 5.f, -5.f);
}

CubeRendererComponent::~CubeRendererComponent()
{
	for (unsigned int i = 0; i < 8; ++i)
	{
		delete cube[i];
	}
}

void CubeRendererComponent::Update(const float& _deltaTime)
{
	// Updating the angle
	angleX += angleXIncrease * _deltaTime;
	angleY += angleYIncrease * _deltaTime;
	angleZ += angleZIncrease * _deltaTime;

	// Updating the scale
	scale += scaleIncrease;
}

void CubeRendererComponent::Render()
{
	CharTexture* tex;
	if (Application::INSTANCE)
		tex = &Application::INSTANCE->GetRenderTexture();

	for (int i = 0; i < 8; ++i)
	{
		// Rotating
		Vec3 rotated = *cube[i];
		rotated.Rotate(*cube[i], Vec3::Forward(), angleX);
		rotated.Rotate(rotated, Vec3::Up(), angleY);
		rotated.Rotate(rotated, Vec3::Right(), angleZ);

		// Scaling
		Vec3 scaled = rotated * scale;

		// Projecting
		Vec3 projected = projection * scaled;

		int x = (int)roundf(projected.x + (tex->GetWidth() * 0.5f));
		int y = (int)roundf(projected.y + (tex->GetHeight() * 0.5f));

		// Doesn't render if the cube vertices exceed the limits of the character map
		if (x < tex->GetWidth() && x > 0 && y < tex->GetHeight() && y > 0)
		{
			(*tex)[x][y] = 1;
		}
	}
}
