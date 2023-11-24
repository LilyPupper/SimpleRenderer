#include "Mesh.h"

#include "Tri.h"

Vertex::Vertex()
	: Position(glm::vec4())
	, Normal(glm::vec3())
{}

Vertex::Vertex(const glm::vec4& _position)
	: Position(_position)
	, Normal(glm::vec3(0.f, 0.f, 0.f))
{}


Vertex::Vertex(const glm::vec4& _position, const glm::vec3& _normal)
	: Position(_position)
	, Normal(_normal)
{}

Mesh::Mesh()
	: MeshID("NULL")
{}

Mesh::~Mesh()
{}

void Mesh::RecalculateSurfaceNormals()
{
	for (unsigned int i = 0; i < Triangles.size(); ++i)
		Triangles[i].RecalculateSurfaceNormal();
}