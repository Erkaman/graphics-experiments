#pragma once

constexpr float kEpsilon = 1e-8;

inline
float deg2rad(const float deg)
{ return deg * M_PI / 180; }


bool rayTriangleIntersect(
    const Vector3f &orig, const Vector3f &dir,
    const Vector3f &v0, const Vector3f &v1, const Vector3f &v2,
    float &t, float &u, float &v)
{
    // compute plane's normal
    Vector3f v0v1 = v1 - v0;
    Vector3f v0v2 = v2 - v0;
    // no need to normalize
    Vector3f N = Vector3f::Cross(v0v1,v0v2); // N
    float denom = Vector3f::Dot(N,N);

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = Vector3f::Dot(N,dir);
    if (fabs(NdotRayDirection) < kEpsilon) // almost 0
        return false; // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    float d = Vector3f::Dot(N,v0);

    // compute t (equation 3)
    t = (  Vector3f::Dot(N,orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind

    // compute the intersection point using equation 1
    Vector3f P = orig + t * dir;

    // Step 2: inside-outside test
    Vector3f C; // vector perpendicular to triangle's plane

    // edge 0
    Vector3f edge0 = v1 - v0;
    Vector3f vp0 = P - v0;
    C = Vector3f::Cross(edge0,vp0);
//    if (Vector3f::Dot(N, C) < 0) return false; // P is on the right side

    // edge 1
    Vector3f edge1 = v2 - v1;
    Vector3f vp1 = P - v1;
    C = Vector3f::Cross(edge1,vp1);
//    if ((u = Vector3f::Dot(N,C)) < 0)  return false; // P is on the right side
    u = Vector3f::Dot(N,C);

    // edge 2
    Vector3f edge2 = v0 - v2;
    Vector3f vp2 = P - v2;
    C = Vector3f::Cross(edge2,vp2);
//    if ((v = Vector3f::Dot(N,C)) < 0) return false; // P is on the right side;
    v = Vector3f::Dot(N,C);

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle
}
