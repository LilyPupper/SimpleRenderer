#include "Components/TransformComponent.h"

TransformComponent::TransformComponent(Object* _owner)
	: Component(_owner), m_Translation(glm::vec3()), m_Rotation(glm::identity<glm::quat>()), m_Scale(glm::vec3(1.f, 1.f, 1.f)), m_TransformationMatrix(glm::identity<glm::mat4>()), m_RebuildMatrix(true)
{
	m_Type = TRANSFORM;
}

TransformComponent::~TransformComponent()
{}

void TransformComponent::Update(const float& _deltaTime)
{}

void TransformComponent::Render()
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
	m_Rotation = glm::rotate(glm::identity<glm::quat>(), glm::radians(_eulerAngles));
	m_RebuildMatrix = true;
}

void TransformComponent::Rotate(const float& _theta, const glm::vec3& _axis)
{
	m_Rotation = glm::rotate(m_Rotation, glm::radians(_theta), glm::normalize(_axis));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateX(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, glm::radians(_theta), glm::vec3(1.0f, 0.0f, 0.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateY(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, glm::radians(_theta), glm::vec3(0.0f, 1.0f, 0.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::RotateZ(const float& _theta)
{
	m_Rotation = glm::rotate(m_Rotation, glm::radians(_theta), glm::vec3(0.0f, 0.0f, 1.0f));
	m_RebuildMatrix = true;
}

void TransformComponent::SetPosition(const glm::vec3& _pos)
{
	SetPosition(_pos.x, _pos.y, _pos.z);
}

void TransformComponent::SetPosition(const float& _x, const float& _y, const float& _z)
{
	m_Translation.x = _x;
	m_Translation.y = _y;
	m_Translation.z = _z;

	m_RebuildMatrix = true;
}

void TransformComponent::Translate(const glm::vec3& _translation)
{
	Translate(_translation.x, _translation.y, _translation.z);
}

void TransformComponent::Translate(const float& _x, const float& _y, const float& _z)
{
	m_Translation.x += _x;
	m_Translation.y += _y;
	m_Translation.z += _z;

	m_RebuildMatrix = true;
}

glm::vec3 TransformComponent::GetForward()
{
	//return glm::vec3{0.0, 0.0, 1.0};
	if (m_RebuildMatrix)
		RebuildTransformation();

	const glm::mat4 inverted = glm::inverse(m_TransformationMatrix);
	const glm::vec3 forward = glm::normalize(glm::vec3(inverted[2]));
	return forward;
}

glm::vec3 TransformComponent::GetUp()
{
	//return glm::vec3{0.0, 1.0, 0.0};
	if (m_RebuildMatrix)
		RebuildTransformation();

	const glm::mat4 inverted = glm::inverse(m_TransformationMatrix);
	const glm::vec3 up = glm::normalize(glm::vec3(inverted[0]));
	return up;
}

glm::vec3 TransformComponent::GetRight()
{
	//return glm::vec3{1.0, 0.0, 0.0};
	if (m_RebuildMatrix)
		RebuildTransformation();

	const glm::mat4 inverted = glm::inverse(m_TransformationMatrix);
	const glm::vec3 right = glm::normalize(glm::vec3(inverted[1]));
	return right;
}

glm::vec3 TransformComponent::GetEuler(bool _degrees/*= true*/) const
{
	return glm::degrees(glm::eulerAngles(m_Rotation));
}

void TransformComponent::RebuildTransformation()
{
	m_ScaleMatrix = glm::mat4(
		m_Scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, m_Scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, m_Scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	m_RotationMatrix = glm::toMat4(m_Rotation);

	m_TranslationMatrix = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.f,
		0.0f, 1.0f, 0.0f, 0.f,
		0.0f, 0.0f, 1.0f, 0.f,
		m_Translation.x, m_Translation.y, m_Translation.z, 1.0f
	);

	m_TransformationMatrix = m_ScaleMatrix * m_RotationMatrix * m_TranslationMatrix;
	m_RebuildMatrix = false;
}

glm::mat4 TransformComponent::GetTransformation()
{
	if (m_RebuildMatrix)
		RebuildTransformation();

	return m_TransformationMatrix;
}