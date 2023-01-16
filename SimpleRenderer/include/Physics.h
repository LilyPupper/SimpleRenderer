#pragma once

class Tri;

#include <glm.hpp>

namespace Physics
{
    bool IntersectRayTriangle(glm::vec3 _origin, glm::vec3 _dir, Tri _tri, float& _baryX, float& _baryY, float& distance);
}