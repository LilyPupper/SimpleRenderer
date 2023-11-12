#pragma once

class Tri;

#include <glm.hpp>

namespace Physics
{
    bool IntersectRayTriangle(const glm::vec3& _origin, const glm::vec3& _dir, const Tri& _tri, float& _baryX, float& _baryY, float& distance);
}