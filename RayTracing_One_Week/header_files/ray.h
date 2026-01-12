#pragma once
#include "utility.h"

class Ray {
    public:
    Ray() {};
    Ray(const Point3& origin, const vec3& direction, double time);
    Ray(const Point3& origin, const vec3& direction);
    const Point3 get_origin() const;
    const vec3 get_direction() const;
    Point3 at(double t) const;

    double time() const { return tm; }


    private:
    Point3 origin;
    vec3 direction;
    double tm;
};