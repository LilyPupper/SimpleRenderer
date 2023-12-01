#pragma once

#include "Mesh.h"
#include "glm/glm.hpp"

namespace Util
{
	void Barycentric(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& u, float& v, float& w);

	void OrderVerticesByYThenX(Vertex& _high, Vertex& _low);
}
