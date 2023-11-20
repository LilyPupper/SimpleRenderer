#pragma once

#include <atomic>

#include "Component.h"

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

class TransformComponent : public Component
{
public:
	TransformComponent(Object* _owner);
	~TransformComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	void SetScale(const float& _scale);
	void SetScale(const float& _x, const float& _y, const float& _z);
	void Scale(const float& _scale);
	void Scale(const float& _x, const float& _y, const float& _z);

	inline glm::vec3 GetScale() const { return m_Scale; }

	// Set rotation in degrees
	void SetRotation(const glm::vec3& _eulerAngles);
	glm::vec3 GetRotation(bool _degrees = true) const;
	glm::quat GetQuaternion() const { return m_Rotation; }

	void Rotate(const float& _theta, const glm::vec3& _axis);
	void RotateX(const float& _theta);
	void RotateY(const float& _theta);
	void RotateZ(const float& _theta);

	void SetPosition(const glm::vec3& _pos);
	void SetPosition(const float& _x, const float& _y, const float& _z);

	inline glm::vec3 GetPosition() const { return m_Translation; }

	void Translate(const glm::vec3& _translation);
	void Translate(const float& _x, const float& _y, const float& _z);

	glm::vec3 GetForward() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetRight() const;

	glm::mat4 GetTranslationMatrix();
	glm::mat4 GetRotationMatrix();
	glm::mat4 GetScaleMatrix();

	void RebuildTransformation();
	glm::mat4 GetTransformation();

private:
	glm::vec3 m_Translation;
	glm::quat m_Rotation;
	glm::vec3 m_Scale;

	glm::mat4 m_TranslationMatrix;
	glm::mat4 m_RotationMatrix;
	glm::mat4 m_ScaleMatrix;

	glm::mat4 m_TransformationMatrix;

	bool m_RebuildMatrix;
};