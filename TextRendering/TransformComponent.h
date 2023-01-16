#pragma once

#include "Component.h"

#include <glm.hpp>
#include <gtx/quaternion.hpp>

class TransformComponent : public Component
{
public:
	TransformComponent(Object* _owner);
	~TransformComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

	void SetScale(const float& _scale);
	void SetScale(const float& _x, const float& _y, const float& _z);
	void Scale(const float& _scale);
	void Scale(const float& _x, const float& _y, const float& _z);

	void SetRotation(const glm::vec3& _eulerAngles);
	void Rotate(const float& _theta, const glm::vec3& _axis);
	void RotateX(const float& _theta);
	void RotateY(const float& _theta);
	void RotateZ(const float& _theta);

	void SetPosition(const float& _x, const float& _y, const float& _z);
	void Translate(const float& _x, const float& _y, const float& _z);

	glm::vec3 GetScale() const;
	glm::vec3 GetPosition() const;
	glm::vec3 GetEuler() const;

	void RebuildTransformation();
	glm::mat4 GetTransformation();

private:
	glm::vec3 m_Translation;
	glm::quat m_Rotation;
	glm::vec3 m_Scale;

	glm::mat4 m_Transformation;

	bool m_RebuildMatrix;
};