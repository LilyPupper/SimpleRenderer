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

	float angleXIncrease = 0.f;
	float angleYIncrease = 0.f;
	float angleZIncrease = 1.f;

	m_Time += _deltaTime;

	TransformComponent* t = static_cast<TransformComponent*>(FindComponentOfType(TRANSFORM));
	if (t != nullptr)
	{
		const glm::vec3 euler = t->GetRotation();
		const glm::vec3 euler1 = t->GetRotation();
		
		glm::vec3 r(angleXIncrease, angleYIncrease, angleZIncrease);

		//t->Rotate(_deltaTime * 25.f, t->GetForward());
		//t->SetRotation(r * m_Time * 15.f);
	}
}

void RotatingModel::Render()
{
	Object::Render();
}