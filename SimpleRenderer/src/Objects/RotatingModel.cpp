#include "Objects/RotatingModel.h"

#include "Mesh.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MeshRendererComponent.h"

RotatingModel::RotatingModel(const char* _filepath)
	: m_Time(0.0f)
{
	AddComponent(new MeshRendererComponent(this));

	// Load mesh
	Mesh* mesh = new Mesh();
	bool loaded = mesh->Load(_filepath);

	// Attach mesh as component
	AddComponent(new MeshComponent(this, mesh));
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
		glm::vec3 scale = t->GetScale() - (sinf(m_Time) * 0.05f);
		t->SetScale(scale.x, scale.y, scale.z);

		glm::vec3 r(angleXIncrease, angleYIncrease, angleZIncrease);
		t->SetRotation(r * m_Time * 1.0f);

		float x = sinf(m_Time) * 0.1f;
		glm::vec3 pos = t->GetPosition();
		t->SetPosition(pos.x + x, pos.y, pos.z);
	}
}

void RotatingModel::Render(CharTexture* _texture)
{
	Object::Render(_texture);
}