#pragma once

#include <glm.hpp>
#include <vec3.hpp>
#include <vec4.hpp>

class Vertex
{
public:
	inline Vertex(const float& _x, const float& _y, const float& _z) : x(_x), y(_y), z(_z) {}
	inline Vertex(const glm::vec3& _v) : x(_v.x), y(_v.y), z(_v.z) {}

	float x, y, z;

	inline operator glm::vec3() const { return glm::vec3(x, y ,z); }
};