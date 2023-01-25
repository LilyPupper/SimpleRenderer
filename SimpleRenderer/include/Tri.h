#pragma once

#include <glm.hpp>

class Tri
{
public:
	Tri(const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3);
	~Tri();
	
	void RecalculateSurfaceNormal();
	glm::vec3 GetSurfaceNormal() const;
	
	Tri operator*(glm::mat4& _m) const;
	Tri operator*(const glm::mat4& _m) const;
	void operator*=(const glm::mat4& _m);

	glm::vec4 v1;
	glm::vec4 v2;
	glm::vec4 v3;

private:
	glm::vec4 m_SurfaceNormal;
};
