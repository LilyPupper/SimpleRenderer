#pragma once

#include "Tri.h"
#include "Vec3.h"

namespace Physics
{
    struct RaycastResult
    {
        RaycastResult()
            :
            intersect(false),
            parameter(0.0f),
            triangleBary{ 0.0f, 0.0f, 0.0f },
            point(Vec3::Zero())
        {
        }

        bool intersect;
        float parameter;
        float triangleBary[3];
        Vec3 point;
    };

    RaycastResult Raycast(const Tri& _tri, const Vec3& _origin, const Vec3& _dir)
    {
        RaycastResult result{};
    
        // Compute the offset origin, edges, and normal.
        Vec3 diff = _origin - _tri.v1;
        Vec3 edge1 = _tri.v2 - _tri.v1;
        Vec3 edge2 = _tri.v3 - _tri.v1;
        Vec3 normal = Vec3::Cross(edge1, edge2);
    
        // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
        // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
        //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
        //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
        //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
        float DdN = Vec3::Dot(_dir, normal);
        float sign = 0.0f;
        if (DdN > 0.0f)
        {
            sign = 1.0f;
        }
        else if (DdN < 0.0f)
        {
            sign = -1.0f;
            DdN = -DdN;
        }
        else
        {
            // Ray and triangle are parallel, call it a "no intersection"
            // even if the ray does intersect.
            result.intersect = false;
            return result;
        }
    
        float DdQxE2 = sign * Vec3::Dot(_dir, Vec3::Cross(diff, edge2));
        if (DdQxE2 >= 0.0f)
        {
            float DdE1xQ = sign * Vec3::Dot(_dir, Vec3::Cross(edge1, diff));
            if (DdE1xQ >= 0.0f)
            {
                if (DdQxE2 + DdE1xQ <= DdN)
                {
                    // Line intersects triangle, check whether ray does.
                    float QdN = -sign * Vec3::Dot(diff, normal);
                    if (QdN >= 0.0f)
                    {
                        // Ray intersects triangle.
                        result.intersect = true;
                        result.parameter = QdN / DdN;
                        result.triangleBary[1] = DdQxE2 / DdN;
                        result.triangleBary[2] = DdE1xQ / DdN;
                        result.triangleBary[0] =
                            1.0f - result.triangleBary[1] - result.triangleBary[2];
                        result.point = _origin + result.parameter * _dir;
                        return result;
                    }
                    // else: t < 0, no intersection
                }
                // else: b1+b2 > 1, no intersection
            }
            // else: b2 < 0, no intersection
        }
        // else: b1 < 0, no intersection
    
        result.intersect = false;
        return result;
    }

    bool IntersectRayTriangle(const Vec3& _origin, const Vec3& _dir, const Tri& _tri, float& _baryX, float& _baryY, float& distance)
    {
        // find vectors for two edges sharing vert0
        Vec3 const edge1 = _tri.v2 - _tri.v1;
        Vec3 const edge2 = _tri.v3 - _tri.v1;

        // begin calculating determinant - also used to calculate U parameter
        Vec3 const p = Vec3::Cross(_dir, edge2);

        // if determinant is near zero, ray lies in plane of triangle
        float const det = Vec3::Dot(edge1, p);

        Vec3 Perpendicular(0);

        if (det > std::numeric_limits<float>::epsilon())
        {
            // calculate distance from vert0 to ray origin
            Vec3 const dist = _origin - _tri.v1;

            // calculate U parameter and test bounds
            _baryX = Vec3::Dot(dist, p);
            if (_baryX < 0.0f || _baryX > det)
                return false;

            // prepare to test V parameter
            Perpendicular = Vec3::Cross(dist, edge1);

            // calculate V parameter and test bounds
            _baryY = Vec3::Dot(_dir, Perpendicular);
            if ((_baryY < 0.0f) || ((_baryX + _baryY) > det))
                return false;
        }
        else if (det < -std::numeric_limits<float>::epsilon())
        {
            // calculate distance from vert0 to ray origin
            Vec3 const dist = _origin - _tri.v1;

            // calculate U parameter and test bounds
            _baryX = Vec3::Dot(dist, p);
            if ((_baryX > 0.0f) || (_baryX < det))
                return false;

            // prepare to test V parameter
            Perpendicular = Vec3::Cross(dist, edge1);

            // calculate V parameter and test bounds
            _baryY = Vec3::Dot(_dir, Perpendicular);
            if ((_baryY > 0.0f) || (_baryX + _baryY < det))
                return false;
        }
        else
            return false; // ray is parallel to the plane of the triangle

        float inv_det = 1.0f / det;

        // calculate distance, ray intersects triangle
        distance = Vec3::Dot(edge2, Perpendicular) * inv_det;
        _baryX *= inv_det;
        _baryY *= inv_det;

        return true;
    }
}