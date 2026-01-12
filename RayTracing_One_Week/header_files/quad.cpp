#include "quad.h"

Quad::Quad(const Point3& Q, const vec3& u, const vec3& v, shared_ptr<Material> mat)
    : Q(Q), u(u), v(v), mat(mat)
{
    auto n = u.cross(v);
    normal = n.normalized();
    D = normal.dot(Q);
    w = n / n.dot(n);

    set_bounding_box();
}

void Quad::set_bounding_box() {
    // Compute the bounding box of all four vertices.
    auto bbox_diagonal1 = AABB(Q, Q + u + v);
    auto bbox_diagonal2 = AABB(Q + u, Q + v);
    bbox = AABB(bbox_diagonal1, bbox_diagonal2);
}

bool Quad::is_interior(double a, double b, Hit_Record &rec) const
{
    Interval unit_interval = Interval(0, 1);
    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.

    if (!unit_interval.contains(a) || !unit_interval.contains(b))
        return false;

    rec.u = a;
    rec.v = b;
    return true;
}

AABB Quad::bounding_box() const
{
    return bbox;
}

bool Quad::hit(const Ray &r, Interval ray_t, Hit_Record &rec) const
{
    auto denom = normal.dot(r.get_direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < 1e-8)
        return false;

    // Return false if the hit point parameter t is outside the ray interval.
    auto t = (D - normal.dot(r.get_origin())) / denom;
    if (!ray_t.contains(t))
        return false;

    auto intersection = r.at(t);
    vec3 planar_hitpt_vector = intersection - Q;
    auto alpha = w.dot(planar_hitpt_vector.cross(v));
    auto beta = w.dot(u.cross(planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
        return false;

    rec.t = t;
    rec.p = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);

    return true;
}
