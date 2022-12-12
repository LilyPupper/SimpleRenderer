#include "RotatingCube.h"

#include "CubeRendererComponent.h"

RotatingCube::RotatingCube()
{
	// Add components here
	AddComponent(new CubeRendererComponent(this));
}

RotatingCube::~RotatingCube()
{}

void RotatingCube::Update(const float& _deltaTime)
{
	Object::Update(_deltaTime);
}

void RotatingCube::Render(CharTexture* _texture)
{
	Object::Render(_texture);
}
