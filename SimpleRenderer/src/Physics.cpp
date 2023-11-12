#include "Physics.h"

#include "Tri.h"

#include <glm.hpp>
#include <limits>

namespace Physics
{
    bool IntersectRayTriangle(const glm::vec3& _origin, const glm::vec3& _dir, const Tri& _tri, float& _baryX, float& _baryY, float& distance)
    {
        // find vectors for two edges sharing vert0
        glm::vec3 edge1 = (glm::vec3)_tri.v2 - (glm::vec3)_tri.v1;
        glm::vec3 edge2 = (glm::vec3)_tri.v3 - (glm::vec3)_tri.v1;

        // begin calculating determinant - also used to calculate U parameter
        glm::vec3 p = glm::cross(_dir, edge2);

        // if determinant is near zero, ray lies in plane of triangle
        float det = glm::dot(edge1, p);

        glm::vec3 Perpendicular(0.0f);

        if (det > std::numeric_limits<float>::epsilon())
        {
            // calculate distance from vert0 to ray origin
            glm::vec3 dist = _origin - (glm::vec3)_tri.v1;

            // calculate U parameter and test bounds
            _baryX = glm::dot(dist, p);
            if (_baryX < 0.0f || _baryX > det)
                return false;

            // prepare to test V parameter
            Perpendicular = glm::cross(dist, edge1);

            // calculate V parameter and test bounds
            _baryY = glm::dot(_dir, Perpendicular);
            if ((_baryY < 0.0f) || ((_baryX + _baryY) > det))
                return false;
        }
        else if (det < -std::numeric_limits<float>::epsilon())
        {
            // calculate distance from vert0 to ray origin
            glm::vec3 dist = _origin - (glm::vec3)_tri.v1;

            // calculate U parameter and test bounds
            _baryX = glm::dot(dist, p);
            if ((_baryX > 0.0f) || (_baryX < det))
                return false;

            // prepare to test V parameter
            Perpendicular = glm::cross(dist, edge1);

            // calculate V parameter and test bounds
            _baryY = glm::dot(_dir, Perpendicular);
            if ((_baryY > 0.0f) || (_baryX + _baryY < det))
                return false;
        }
        else
            return false; // ray is parallel to the plane of the triangle

        float inv_det = 1.0f / det;

        // calculate distance, ray intersects triangle
        distance = glm::dot(edge2, Perpendicular) * inv_det;
        _baryX *= inv_det;
        _baryY *= inv_det;

        return true;
    }
}