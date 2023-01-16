#include "Objects/RotatingModel.h"

#include "Mesh.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/MeshRendererComponent.h"

#include <string>
#include <Windows.h>

RotatingModel::RotatingModel(const char* _filepath)
	: m_Time(0.0f)
{
	AddComponent(new MeshRendererComponent(this));

	// Get current exe path
	char cCurrentPath[FILENAME_MAX];
	GetModuleFileNameA(NULL, cCurrentPath, sizeof(cCurrentPath));
	std::string directory = cCurrentPath;
	directory = directory.substr(0, directory.find_last_of("\\"));

	// Load mesh
	Mesh* mesh = new Mesh();
	bool loaded = mesh->Load((directory + "\\" + _filepath).c_str());

	// Attach mesh as component
	AddComponent(new MeshComponent(this, mesh));

	TransformComponent* t = static_cast<TransformComponent*>(FindComponentOfType(TRANSFORM));
	if (t != nullptr)
	{
		t->SetScale(10.0f);
		t->SetPosition(60.0f, 15.0f, 0.0f);
	}
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

		float x = sinf(m_Time) * 10.0f;
		t->SetPosition(60.0f + x, 15.0f, 0.0f);
	}
}

void RotatingModel::Render(CharTexture* _texture)
{
	Object::Render(_texture);
}