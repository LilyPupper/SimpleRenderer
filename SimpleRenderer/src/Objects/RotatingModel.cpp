#include "Objects/RotatingModel.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Renderer/RendererBase.h"

RotatingModel::RotatingModel()
	: m_Time(0.0f)
{}

RotatingModel::~RotatingModel()
{}

void RotatingModel::Update(const float& _deltaTime)
{
	Object::Update(_deltaTime);

	float angleXIncrease = 1.5f;
	float angleYIncrease = 0.75f;
	float angleZIncrease = 0.5f;

	m_Time += _deltaTime;

	TransformComponent* t = static_cast<TransformComponent*>(FindComponentOfType(TRANSFORM));
	if (t != nullptr)
	{
		const glm::vec3 euler = t->GetEuler();
		const glm::vec3 euler1 = t->GetEuler();
		
		glm::vec3 r(angleXIncrease, angleYIncrease, angleZIncrease);
		
		t->SetRotation(r * m_Time * 15.f);
	}
}

void RotatingModel::Render()
{
	Object::Render();
}