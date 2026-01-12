#pragma once

#include"hittable.h"

class Sphere : public Hittable {
    public:
        Sphere(const Point3& static_center, double radius, shared_ptr<Material> mat);
        Sphere(const Point3& center1, const Point3& center2, double radius, shared_ptr<Material> mat);
        bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override;
        AABB bounding_box() const override;
    
    private:
        double radius;
        shared_ptr<Material> mat;
        AABB bbox;
        Ray center;

        static void get_sphere_uv(const Point3& p, double& u, double& v);
};