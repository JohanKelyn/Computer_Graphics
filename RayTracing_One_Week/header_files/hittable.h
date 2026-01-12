#pragma once

#include "aabb.h"

class Material;

struct Hit_Record {
    Point3 p;
    vec3 normal;
    shared_ptr<Material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    void set_face_normal(const Ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = r.get_direction().dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
    public:
        virtual ~Hittable() = default;
        virtual bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const = 0;
        virtual AABB bounding_box() const = 0;
};