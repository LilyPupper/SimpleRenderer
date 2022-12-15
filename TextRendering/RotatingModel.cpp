#include "RotatingModel.h"

#include "TransformComponent.h"
#include "Mesh.h"
#include "MeshComponent.h"
#include "MeshRendererComponent.h"

#include <string>
#include <Windows.h>

RotatingModel::RotatingModel(const char* _filepath)
{
	AddComponent(new MeshRendererComponent(this));
	
	// Get current exe path
	char cCurrentPath[FILENAME_MAX];
	GetModuleFileNameA(NULL, cCurrentPath, sizeof(cCurrentPath));
	std::string directory = cCurrentPath;
	directory = directory.substr(0, directory.find_last_of("\\"));

	// Load mesh
	Mesh* mesh = new Mesh();
	bool loaded = mesh->Load((directory + "\\..\\..\\..\\models\\" + _filepath).c_str());

	// Attach mesh as component
	AddComponent(new MeshComponent(this, mesh));
}

RotatingModel::~RotatingModel()
{}

void RotatingModel::Update(const float& _deltaTime)
{
	Object::Update(_deltaTime);
}

void RotatingModel::Render(CharTexture* _texture)
{
	Object::Render(_texture);
}