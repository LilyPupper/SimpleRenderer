#pragma once

#include "glm/glm.hpp"

namespace Util
{
	void Barycentric(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& u, float& v, float& w);

	void OrderPointsByYThenX(glm::vec4& _high, glm::vec4& _low);
}
