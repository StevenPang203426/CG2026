#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    _normal = normal.normalized();
    _d = d;
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
    float denom = Vector3f::dot(_normal, r.getDirection());
    if (fabs(denom) < 1e-8f) {
        return false; // ray parallel to plane
    }
    float t = (_d - Vector3f::dot(_normal, r.getOrigin())) / denom;
    if (t > tmin && t < h.getT()) {
        h.set(t, this->material, _normal);
        return true;
    }
    return false;
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const
{
    // Moller-Trumbore algorithm
    Vector3f e1 = _v[1] - _v[0];
    Vector3f e2 = _v[2] - _v[0];
    Vector3f s = r.getOrigin() - _v[0];

    // Solve: [-d, e1, e2] * [t, beta, gamma]^T = s
    Matrix3f A(-r.getDirection(), e1, e2, true); // columns
    float detA = A.determinant();
    if (fabs(detA) < 1e-8f) {
        return false;
    }
    Vector3f x = A.inverse() * s;

    float t = x[0];
    float beta = x[1];
    float gamma = x[2];

    if (beta < 0 || gamma < 0 || beta + gamma > 1) {
        return false;
    }
    if (t > tmin && t < h.getT()) {
        float alpha = 1.0f - beta - gamma;
        Vector3f normal = (alpha * _normals[0] + beta * _normals[1] + gamma * _normals[2]).normalized();
        h.set(t, this->material, normal);
        return true;
    }
    return false;
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
    _m = m;
    _m_inv = m.inverse();
}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
    // Transform ray from world to local coordinates
    Vector4f origin4(_m_inv * Vector4f(r.getOrigin(), 1.0f));
    Vector4f dir4(_m_inv * Vector4f(r.getDirection(), 0.0f));

    Vector3f localOrigin = origin4.xyz();
    Vector3f localDir = dir4.xyz();

    Ray localRay(localOrigin, localDir);

    if (_object->intersect(localRay, tmin, h)) {
        // Transform normal back to world coordinates
        // Normal transforms by inverse-transpose of M
        Vector3f localNormal = h.getNormal();
        Vector4f worldNormal4 = _m_inv.transposed() * Vector4f(localNormal, 0.0f);
        Vector3f worldNormal = worldNormal4.xyz().normalized();
        h.set(h.getT(), h.getMaterial(), worldNormal);
        return true;
    }
    return false;
}