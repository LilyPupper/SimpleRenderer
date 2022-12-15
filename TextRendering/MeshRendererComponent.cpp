#include "MeshRendererComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"

#include "CharTexture.h"
#include "Physics.h"
#include "Mesh.h"

#include <future>

MeshRendererComponent::MeshRendererComponent(Object* _owner)
	: Component(_owner), m_Projection(Mat3::Identity()), m_RotAngles(Vec3()), m_Scale(1.0f), m_Time(0.0f)
{
	m_Type = MESHRENDERER;
}

MeshRendererComponent::~MeshRendererComponent()
{}

void MeshRendererComponent::Update(const float& _deltaTime)
{
	float angleXIncrease = 1.5f;
	float angleYIncrease = 0.75f;
	float angleZIncrease = 0.5f;

	// Updating the scale
	m_Time += _deltaTime;
	m_Scale = m_Scale + (sinf(m_Time * 2.0f) * 2.0f);

	// Updating the angle
	m_RotAngles.x += angleXIncrease * _deltaTime;
	m_RotAngles.y += angleYIncrease * _deltaTime;
	m_RotAngles.z += angleZIncrease * _deltaTime;

	// Updating the position
	TransformComponent* transform = GetTransform();
	if (transform)
	{
		transform->m_Position = Vec3(transform->m_Position.x + sinf(m_Time), transform->m_Position.y, transform->m_Position.z);
	}
}

void MeshRendererComponent::Render(CharTexture* _texture)
{
	MeshComponent* meshComp = static_cast<MeshComponent*>(m_Owner->FindComponentOfType(MESH));
	if (meshComp && meshComp->IsActive())
	{
		//Render_Single(_texture);
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
			Vec3 origin(x, y, -10.0f);
			Vec3 dir(0.0f, 0.0f, 1.0f);

			// Raycast every triangle
			for (unsigned int i = 0; i < _mesh->m_Triangles.size(); ++i)
			{
				Tri t = _mesh->m_Triangles[i];

				// Scale
				t = t * m_Scale;

				// Rotate
				t.RotatePoints(Vec3::Forward(), m_RotAngles.x);
				t.RotatePoints(Vec3::Up(), m_RotAngles.y);
				t.RotatePoints(Vec3::Right(), m_RotAngles.z);

				// Translate
				TransformComponent* transform = GetTransform();
				if (transform)
				{
					t = t + transform->m_Position;
				}

				// Raycast
				float baryX, baryY, distance;
				if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
				{
					// Depth test
					if (distance < (*_texture)[x][y].Depth)
					{
						(*_texture)[x][y].Data = 1;
						(*_texture)[x][y].Depth = distance;
						(*_texture)[x][y].Color = MESH_COLOUR::RED;
					}
				}
			}

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
	// For every column in the texture
	for (unsigned int y = 0; y < _texture->GetHeight(); ++y)
	{
		if ((*_texture)[_row][y].Data == 1)
			continue;

		Vec3 origin(_row, y, -10.0f);
		Vec3 dir(0.0f, 0.0f, 1.0f);

		// Raycast every triangle
		for (unsigned int i = 0; i < _mesh->m_Triangles.size(); ++i)
		{
			Tri t = _mesh->m_Triangles[i];

			// Scale
			t = t * m_Scale;

			// Rotate
			t.RotatePoints(Vec3::Forward(), m_RotAngles.x);
			t.RotatePoints(Vec3::Up(), m_RotAngles.y);
			t.RotatePoints(Vec3::Right(), m_RotAngles.z);

			// Translate
			TransformComponent* transform = GetTransform();
			if (transform)
			{
				t = t + transform->m_Position;
			}

			// Raycast
			float baryX, baryY, distance;
			if (Physics::IntersectRayTriangle(origin, dir, t, baryX, baryY, distance))
			{
				// Depth test
				if (distance < (*_texture)[_row][y].Depth)
				{
					(*_texture)[_row][y].Data = 1;
					(*_texture)[_row][y].Depth = distance;
					(*_texture)[_row][y].Color = MESH_COLOUR::RED;
				}
			}
		}
	}
}