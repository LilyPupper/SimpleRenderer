#pragma once

#include "Tri.h"
#include "Vec3.h"

namespace Physics
{
    bool Raycast(const Tri& _tri, const Vec3& _origin, const Vec3& _dir)
    {
        Vec3 e1, e2, pvec, qvec, tvec;

        e1 = _tri.v2 - _tri.v1;
        e2 = _tri.v3 - _tri.v1;
        pvec = Vec3::Cross(_dir, e2);

        Vec3 dirNormal = _dir.Normalized();

        float det = Vec3::Dot(pvec, e1);

        if (det != 0.0f)
        {
            float invDet = 1.0f / det;
            tvec = _origin - _tri.v1;

            float u = invDet * Vec3::Dot(tvec, pvec);
            if (u < 0.0f || u > 1.0f)
            {

                return false;
            }
            qvec = Vec3::Cross(tvec, e1);

            float v = invDet * Vec3::Dot(qvec, _dir);
            if (v < 0.0f || u + v > 1.0f)
            {

                return false;
            }
        }
        else return false;
        return true;
    }
}