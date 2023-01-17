#pragma once

#include "Component.h"

class MeshComponent : public Component
{
public:
	MeshComponent(Object* _owner, const char* _meshID);
	~MeshComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

	inline const char* GetMeshID() { return m_MeshID; }

private:
	const char* m_MeshID;
};