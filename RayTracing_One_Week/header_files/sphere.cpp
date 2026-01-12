#include "sphere.h"

// Stationary Sphere
Sphere::Sphere(const Point3& static_center, double radius, shared_ptr<Material> mat)
: center(static_center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat(mat)
{
    auto rvec = vec3(radius, radius, radius);
    bbox = AABB(static_center - rvec, static_center + rvec);
}


Sphere::Sphere(const Point3& center1, const Point3& center2, double radius, shared_ptr<Material> mat)
: center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat)
{
    auto rvec = vec3(radius, radius, radius);
    AABB box1(center.at(0) - rvec, center.at(0) + rvec);
    AABB box2(center.at(1) - rvec, center.at(1) + rvec);
    bbox = AABB(box1, box2);
}

AABB Sphere::bounding_box() const { return bbox; }


bool Sphere::hit(const Ray& r, Interval ray_t, Hit_Record& rec) const
{
    Point3 current_center = center.at(r.time());
    vec3 oc = current_center - r.get_origin();
    auto a = r.get_direction().squaredNorm();
    auto h = r.get_direction().dot(oc);
    auto c = oc.squaredNorm() - radius*radius;

    auto discriminant = h*h - a*c;
    if (discriminant < 0)
        return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - current_center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;


    return true;
}

void Sphere::get_sphere_uv(const Point3 &p, double &u, double &v) {
    auto theta = std::acos(-p.y());
    auto phi = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2*pi);
    v = theta / pi;
}

