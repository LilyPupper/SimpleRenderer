#pragma once

#include <atomic>

#include "Component.h"

#include <glm.hpp>
#include <gtx/quaternion.hpp>

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

	// Set rotation in degrees
	void SetRotation(const glm::vec3& _eulerAngles);
	void Rotate(const float& _theta, const glm::vec3& _axis);
	void RotateX(const float& _theta);
	void RotateY(const float& _theta);
	void RotateZ(const float& _theta);

	void SetPosition(const glm::vec3& _pos);
	void SetPosition(const float& _x, const float& _y, const float& _z);

	void Translate(const glm::vec3& _translation);
	void Translate(const float& _x, const float& _y, const float& _z);

	inline glm::vec3 GetScale() const { return m_Scale; }
	inline glm::mat4 GetScaleMatrix() const { return m_ScaleMatrix; }
	
	inline glm::vec3 GetPosition() const { return m_Translation; }
	inline glm::mat4 GetPositionMatrix() const { return m_TransformationMatrix; }

	glm::vec3 GetForward();
	glm::vec3 GetUp();
	glm::vec3 GetRight();

	glm::vec3 GetEuler(bool _degrees = true) const;
	inline glm::mat4 GetRotationMatrix() const { return m_RotationMatrix; }

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