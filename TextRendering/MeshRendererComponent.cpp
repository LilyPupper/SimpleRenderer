#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "Tri.h"

#include "CharTexture.h"
#include "Physics.h"
#include "Mesh.h"

#include <vec4.hpp>
#include <future>

MeshRendererComponent::MeshRendererComponent(Object* _owner, const RENDER_MODE& _mode)
	: Component(_owner), m_Mode(_mode)
{
	m_Type = MESHRENDERER;
}

MeshRendererComponent::~MeshRendererComponent()
{}

void MeshRendererComponent::Update(const float& _deltaTime)
{}

void MeshRendererComponent::Render(CharTexture* _texture)
{
	MeshComponent* meshComp = static_cast<MeshComponent*>(m_Owner->FindComponentOfType(MESH));
	if (meshComp && meshComp->IsActive())
	{
		if(m_Mode == RENDER_MODE::SINGLE)
			Render_Single(_texture, meshComp->GetMesh());
		else
			Render_Async(_texture, meshComp->GetMesh());
	}
}

void MeshRendererComponent::Render_Single(CharTexture* _texture, const Mesh* _mesh)
{
	// For every pixel in the texture
	for (unsigned int y = 0; y < _texture->GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < _texture->GetWidth(); ++x)
		{
			RenderPixel(_texture, _mesh, y, x);
		}
	}
}

void MeshRendererComponent::Render_Async(CharTexture* _texture, const Mesh* _mesh)
{
	for (unsigned int x = 0; x < _texture->GetWidth(); ++x)
	{
		m_Futures.push_back(std::async(std::launch::async, &MeshRendererComponent::RenderRow, this, _texture, _mesh, x));
	}

	bool finished = false;
	while (!finished)
	{
		int count = 0;
		for (unsigned int i = 0; i < m_Futures.size(); ++i)
		{
			if (m_Futures[i]._Is_ready())
				++count;
		}

		if (count == m_Futures.size())
		{
			finished = true;
			m_Futures.clear();
		}
	}
}

void MeshRendererComponent::RenderRow(CharTexture* _texture, const Mesh* _mesh, unsigned int _row)
{
	for (unsigned int y = 0; y < _texture->GetHeight(); ++y)
	{
		if ((*_texture)[_row][y].Data == 1)
			continue;

		RenderPixel(_texture, _mesh, y, _row);
	}
}

void MeshRendererComponent::RenderPixel(CharTexture* _texture, const Mesh* _mesh, unsigned int _column, unsigned int _row)
{
	glm::vec4 origin((float)_row, (float)_column, -10.0f, 1.0f);
	glm::vec4 dir(0.0f, 0.0f, 1.0f, 0.0f);

	// Raycast every triangle
	for (unsigned int i = 0; i < _mesh->m_Triangles.size(); ++i)
	{
		Tri t = _mesh->m_Triangles[i];

		// Apply transformation matrix
		TransformComponent* transform = GetTransform();
		if (transform)
		{
			t = t * transform->GetTransformation();
		}
		
		// Raycast
		float baryX, baryY, distance;
		if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
		{
			// Depth test
			if (distance < (*_texture)[_row][_column].Depth)
			{
				(*_texture)[_row][_column].Data = 1;
				(*_texture)[_row][_column].Depth = distance;
			}
		}
	}
}