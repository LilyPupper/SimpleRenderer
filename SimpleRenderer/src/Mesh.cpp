#include "Mesh.h"

#include "Tri.h"

Mesh::Mesh()
	: m_MeshID("NULL")
{}

Mesh::~Mesh()
{}

void Mesh::RecalculateSurfaceNormals()
{
	for (unsigned int i = 0; i < m_Triangles.size(); ++i)
		m_Triangles[i].RecalculateSurfaceNormal();
}