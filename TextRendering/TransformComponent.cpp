#include "TransformComponent.h"

#include "CharTexture.h"

TransformComponent::TransformComponent(Object* _owner)
	: Component(_owner), m_Translation(glm::vec3()), m_Rotation(glm::identity<glm::quat>()), m_Scale(glm::vec3()), m_Transformation(glm::identity<glm::mat4>()), m_RebuildMatrix(true)
{
	m_Type = TRANSFORM;
}

TransformComponent::~TransformComponent()
{}

void TransformComponent::Update(const float& _deltaTime)
{}

void TransformComponent::Render(CharTexture* _texture)
{}

void TransformComponent::SetScale(const float& _scale)
{
	m_Scale = glm::vec3(_scale, _scale, _scale);
	m_RebuildMatrix = true;
}

void TransformComponent::SetScale(const float& _x, const float& _y, const float& _z)
{
	m_Scale = glm::vec3(_x, _y, _z);
	m_RebuildMatrix = true;
}

void TransformComponent::Scale(const float& _scale)
{
	m_Scale *= _scale;
	m_RebuildMatrix = true;
}

void TransformComponent::Scale(const float& _x, const float& _y, const float& _z)
{
	m_Scale *= glm::vec3(_x, _y, _z);
	m_RebuildMatrix = true;
}

void TransformComponent::SetRotation(const glm::vec3& _eulerAngles)
{
	m_Rotation = glm::rotate(glm::identity<glm::quat>(), _eulerAngles);
	m_RebuildMatrix = true;
}

void TransformComponent::Rotate(const float& _theta, const glm::vec3& _axis)
{
	m_Rotation = glm::rotate(m_Rotation, _theta, glm::normalize(_axis));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateX(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, _theta, glm::vec3(1.0f, 0.0f, 0.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateY(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, _theta, glm::vec3(0.0f, 1.0f, 0.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateZ(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, _theta, glm::vec3(0.0f, 0.0f, 1.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::SetPosition(const float& _x, const float& _y, const float& _z)
{
	m_Translation.x = _x;
	m_Translation.y = _y;
	m_Translation.z = _z;

	m_RebuildMatrix = true;
}

void TransformComponent::Translate(const float& _x, const float& _y, const float& _z)
{
	m_Translation.x += _x;
	m_Translation.y += _y;
	m_Translation.z += _z;

	m_RebuildMatrix = true;
}

glm::vec3 TransformComponent::GetScale() const
{
	return m_Scale;
}

glm::vec3 TransformComponent::GetPosition() const
{
	return m_Translation;
}

glm::vec3 TransformComponent::GetEuler() const
{
	return glm::eulerAngles(m_Rotation);
}

void TransformComponent::RebuildTransformation()
{
	glm::mat4 scale(
		m_Scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, m_Scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, m_Scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotation = glm::toMat4(m_Rotation);

	glm::mat4 translation(
		1.0f, 0.0f, 0.0f, m_Translation.x,
		0.0f, 1.0f, 0.0f, m_Translation.y,
		0.0f, 0.0f, 1.0f, m_Translation.z,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	m_Transformation = scale * rotation * translation;

	m_RebuildMatrix = false;
}

glm::mat4 TransformComponent::GetTransformation()
{
	if (m_RebuildMatrix)
		RebuildTransformation();

	return m_Transformation;
}