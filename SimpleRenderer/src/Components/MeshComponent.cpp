#include "Components/MeshComponent.h"

#include "Renderer/Renderer.h"

MeshComponent::MeshComponent(Object* _owner, Renderer* _renderer, const char* meshID)
	: Component(_owner)
	, bVisible(true)
	, _Renderer(_renderer)
{
	m_Type = MESH;

	if (_Renderer)
	{
		MeshID = _Renderer->RegisterMesh(meshID);
	}
	else
	{
		bVisible = false;
	}
}

MeshComponent::~MeshComponent()
{}

void MeshComponent::Update(const float& _deltaTime)
{}

void MeshComponent::Render()
{
	if (!bVisible)
	{
		return;
	}

	TransformComponent* t = GetTransform();
	if (!t)
	{
		return;
	}

	_Renderer->DrawMesh(MeshID, t);
}
