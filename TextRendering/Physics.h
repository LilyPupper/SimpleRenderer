#pragma once

#include "Tri.h"
#include "Vec3.h"

namespace Physics
{
    bool IntersectRayTriangle(const Vec3& _origin, const Vec3& _dir, const Tri& _tri, float& _baryX, float& _baryY, float& distance);
}