#include "Objects/RotatingModel.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Renderer/RendererBase.h"

RotatingModel::RotatingModel(Object* _parent /*= nullptr*/)
	: Object(_parent)
	, Time(0.0f)
{}

RotatingModel::~RotatingModel()
{}

void RotatingModel::Update(const float& _deltaTime)
{
	Object::Update(_deltaTime);

	float angleXIncrease = 0.25f;
	float angleYIncrease = 0.75f;
	float angleZIncrease = 0.325f;

	Time += _deltaTime;

	TransformComponent* t = static_cast<TransformComponent*>(FindComponentOfType(TRANSFORM));
	if (t != nullptr)
	{
		const glm::vec3 euler = t->GetRotation();
		const glm::vec3 euler1 = t->GetRotation();
		
		glm::vec3 r(angleXIncrease, angleYIncrease, angleZIncrease);

		const float rotSpeed = 15.f;
		t->Rotate(_deltaTime * rotSpeed, t->GetForward());
	}
}

void RotatingModel::Render()
{
	Object::Render();
}