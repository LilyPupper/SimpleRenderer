#include "Objects/RotatingModel.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MeshRendererComponent.h"
#include "Renderer/Renderer.h"

RotatingModel::RotatingModel(Renderer* _renderer, const char* _filepath)
	: m_Renderer(_renderer), m_Time(0.0f)
{
	AddComponent(new MeshRendererComponent(this, m_Renderer));

	// Load mesh
	const char* meshID = m_Renderer->RegisterMesh(_filepath);

	// Attach mesh as component
	AddComponent(new MeshComponent(this, meshID));
}

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
		//glm::vec3 scale = t->GetScale() - (sinf(m_Time) * 0.5f);
		//t->SetScale(scale.x, scale.y, scale.z);
		
		glm::vec3 r(angleXIncrease, angleYIncrease, angleZIncrease);
		t->SetRotation(r * m_Time * 0.5f);

		//float x = sinf(m_Time);
		//glm::vec3 pos = t->GetPosition();
		//t->SetPosition(pos.x + x, pos.y, pos.z);
	}
}

void RotatingModel::Render()
{
	Object::Render();
}