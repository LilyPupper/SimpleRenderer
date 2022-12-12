#include "CubeRendererComponent.h"

#include "Application.h"
#include "CharTexture.h"
#include "Physics.h"

#include "TransformComponent.h"

CubeRendererComponent::CubeRendererComponent(Object* _owner)
	: Component(_owner), m_Projection(Mat3::Identity()), m_RotAngles(Vec3()), m_Scale(1.0f), m_Time(0.0f)
{
	TransformComponent* transform = GetTransform();
	if (transform)
	{
		transform->m_Position = Vec3(60.0f, 15.0f, 0.0f);
	}

	// Create vertices
	m_Vertices.push_back(Vec3(-1.f, -1.f,  1.f));	// 1
	m_Vertices.push_back(Vec3( 1.f, -1.f,  1.f));	// 2
	m_Vertices.push_back(Vec3( 1.f,  1.f,  1.f));	// 3
	m_Vertices.push_back(Vec3(-1.f,  1.f,  1.f));	// 4

	m_Vertices.push_back(Vec3(-1.f, -1.f, -1.f));	// 5
	m_Vertices.push_back(Vec3( 1.f, -1.f, -1.f));	// 6
	m_Vertices.push_back(Vec3( 1.f,  1.f, -1.f));	// 7
	m_Vertices.push_back(Vec3(-1.f,  1.f, -1.f));	// 8

	// Building triangles
	// Front face		1	2
	// 1 - 2 - 4
	// 3 - 4 - 2		4	3
	m_Triangles.push_back(Tri(m_Vertices[0], m_Vertices[1], m_Vertices[3]));
	m_Triangles.push_back(Tri(m_Vertices[2], m_Vertices[3], m_Vertices[1]));
	// Right face		2	6
	// 2 - 6 - 3
	// 7 - 3 - 6		3	7
	m_Triangles.push_back(Tri(m_Vertices[1], m_Vertices[5], m_Vertices[2]));
	m_Triangles.push_back(Tri(m_Vertices[6], m_Vertices[2], m_Vertices[5]));
	// Back face		6	5
	// 6 - 5 - 7
	// 8 - 7 - 5		7	8
	m_Triangles.push_back(Tri(m_Vertices[5], m_Vertices[4], m_Vertices[6]));
	m_Triangles.push_back(Tri(m_Vertices[7], m_Vertices[6], m_Vertices[4]));
	// Left face		5	1
	// 5 - 1 - 8
	// 4 - 8 - 1		8	4
	m_Triangles.push_back(Tri(m_Vertices[4], m_Vertices[0], m_Vertices[7]));
	m_Triangles.push_back(Tri(m_Vertices[3], m_Vertices[7], m_Vertices[0]));
	// Top face			5	6
	// 5 - 6 - 1
	// 2 - 1 - 6		1	2
	m_Triangles.push_back(Tri(m_Vertices[4], m_Vertices[5], m_Vertices[0]));
	m_Triangles.push_back(Tri(m_Vertices[1], m_Vertices[0], m_Vertices[5]));
	// Bottom face		4	3
	// 4 - 3 - 8
	// 7 - 8 - 3		8	7
	m_Triangles.push_back(Tri(m_Vertices[3], m_Vertices[2], m_Vertices[7]));
	m_Triangles.push_back(Tri(m_Vertices[6], m_Vertices[7], m_Vertices[2]));
}

CubeRendererComponent::~CubeRendererComponent()
{
	//for (unsigned int i = 0; i < m_Vertices.size(); ++i)
	//	delete m_Vertices[i];
	//
	//for (unsigned int i = 0; i < m_Triangles.size(); ++i)
	//	delete m_Triangles[i];
}

void CubeRendererComponent::Update(const float& _deltaTime)
{
	float angleXIncrease = 1.5f;
	float angleYIncrease = 0.75f;
	float angleZIncrease = 0.5f;

	// Updating the scale
	m_Time += _deltaTime;
	m_Scale = 5.0f + (sinf(m_Time * 0.25f) * 2.0f);

	// Updating the angle
	m_RotAngles.x += angleXIncrease * _deltaTime;
	m_RotAngles.y += angleYIncrease * _deltaTime;
	m_RotAngles.z += angleZIncrease * _deltaTime;

	// Updating the position
	TransformComponent* transform = GetTransform();
	if (transform)
	{
		transform->m_Position = Vec3(60.0f + (sinf(m_Time) * 20.0f), 15.0f, 0.0f);
	}
}

void CubeRendererComponent::Render(CharTexture* _texture)
{	
	// For every pixel in the texture
	for (unsigned int y = 0; y < _texture->GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < _texture->GetWidth(); ++x)
		{
			Vec3 origin(x, y, -15.0f);
			Vec3 dir(0.0f, 0.0f, 1.0f);
	
			// Raycast every triangle
			for (unsigned int i = 0; i < m_Triangles.size(); ++i)
			{
				//if (m_Triangles[i].Cull(dir))
				//	break;

				Tri t = m_Triangles[i];
				
				// Scale
				t = t * m_Scale;
	
				// Rotate
				t.RotatePoints(Vec3::Forward(),		m_RotAngles.x);
				t.RotatePoints(Vec3::Up(),			m_RotAngles.y);
				t.RotatePoints(Vec3::Right(),		m_RotAngles.z);
				
				// Translate
				TransformComponent* transform = GetTransform();
				if (transform)
				{
					t = t + transform->m_Position;
				}
				
				bool hit = Physics::Raycast(t, origin, dir);
				if (hit)
				{
					(*_texture)[x][y].Data = 1;
				}
			}
	
		}
	}

	//for (int i = 0; i < m_Vertices.size(); ++i)
	//{
	//	Vec3 result = m_Vertices[i];
	//
	//	// Scaling
	//	result = result * m_Scale;
	//
	//	// Rotating
	//	result.Rotate(Vec3::Forward(), m_RotAngles.x);
	//	result.Rotate(Vec3::Up(), m_RotAngles.y);
	//	result.Rotate(Vec3::Right(), m_RotAngles.z);
	//
	//	// Translate
	//	TransformComponent* transform = GetTransform();
	//	if (transform)
	//	{
	//		result = result + transform->m_Position;
	//	}
	//
	//	// Projecting
	//	//result = m_Projection * result;
	//
	//	int x = (int)roundf(result.x);
	//	int y = (int)roundf(result.y);
	//
	//	// Doesn't render if the cube vertices exceed the limits of the character map
	//	if (x < _texture->GetWidth() && x > 0 && y < _texture->GetHeight() && y > 0)
	//	{
	//		(*_texture)[x][y].Data = 2;
	//	}
	//}
}
