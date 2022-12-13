#include "RotatingCube.h"

#include "TransformComponent.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshRendererComponent.h"

#include <string>

RotatingCube::RotatingCube()
{
	TransformComponent* transform = GetTransform();
	if (transform)
	{
		transform->m_Position = Vec3(60.0f, 15.0f, 0.0f);
	}

	AddComponent(new MeshRendererComponent(this));

	Mesh* mesh = new Mesh();
	bool loaded = mesh->Load("C:/Projects/TextRendering/models/Torus_High.obj");
	AddComponent(new MeshComponent(this, mesh));
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
