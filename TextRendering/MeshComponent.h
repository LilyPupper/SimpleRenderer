#pragma once

#include "Component.h"
#include "Mesh.h"

class MeshComponent : public Component
{
public:
	MeshComponent(Object* _owner, Mesh* _mesh);
	~MeshComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

	inline const Mesh* GetMesh() { return m_Mesh; }

private:
	Mesh* m_Mesh;
};