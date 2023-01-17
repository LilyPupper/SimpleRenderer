#include "Components/MeshComponent.h"

MeshComponent::MeshComponent(Object* _owner, const char* _meshID)
	: Component(_owner), m_MeshID(_meshID)
{
	m_Type = MESH;
}

MeshComponent::~MeshComponent()
{}

void MeshComponent::Update(const float& _deltaTime)
{}

void MeshComponent::Render()
{}
