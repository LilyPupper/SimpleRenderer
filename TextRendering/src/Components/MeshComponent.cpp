#include "Components/MeshComponent.h"

#include "Mesh.h"

MeshComponent::MeshComponent(Object* _owner, Mesh* _mesh)
	: Component(_owner), m_Mesh(_mesh)
{
	m_Type = MESH;
}

MeshComponent::~MeshComponent()
{
	if (m_Mesh)
		delete m_Mesh;
}

void MeshComponent::Update(const float& _deltaTime)
{}

void MeshComponent::Render(CharTexture* _texture)
{}
