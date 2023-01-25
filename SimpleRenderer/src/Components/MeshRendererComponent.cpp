#include "Components/MeshRendererComponent.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Renderer/Renderer.h"

#include <glm.hpp>

MeshRendererComponent::MeshRendererComponent(Object* _owner, Renderer* _renderer)
	: Component(_owner), m_Renderer(_renderer)
{
	m_Type = MESHRENDERER;
}

MeshRendererComponent::~MeshRendererComponent()
{}

void MeshRendererComponent::Update(const float& _deltaTime)
{}

void MeshRendererComponent::Render()
{
	MeshComponent* meshComp = static_cast<MeshComponent*>(m_Owner->FindComponentOfType(MESH));
	TransformComponent* t = GetTransform();
	if (meshComp && meshComp->IsActive() && t)
	{
		m_Renderer->DrawMesh(meshComp->GetMeshID(), t);
	}
}