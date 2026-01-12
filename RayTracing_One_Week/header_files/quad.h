#pragma once

#include "hittable.h"

class Quad : public Hittable {
    public:
        Quad(const Point3& Q, const vec3& u, const vec3& v, shared_ptr<Material> mat);

        virtual void set_bounding_box();

        virtual bool is_interior(double a, double b, Hit_Record& rec) const;

        AABB bounding_box() const override;;

        bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override;

    private:
        Point3 Q;
        vec3 u, v;
        vec3 w;
        shared_ptr<Material> mat;
        AABB bbox;
        vec3 normal;
        double D;
};