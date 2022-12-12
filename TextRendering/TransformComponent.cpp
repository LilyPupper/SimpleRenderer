#include "TransformComponent.h"

#include "CharTexture.h"

TransformComponent::TransformComponent(Object* _owner)
	: Component(_owner), m_Position(0.f)
{
	m_Type = TRANSFORM;
}

TransformComponent::~TransformComponent()
{}

void TransformComponent::Update(const float& _deltaTime)
{}

void TransformComponent::Render(CharTexture* _texture)
{}
