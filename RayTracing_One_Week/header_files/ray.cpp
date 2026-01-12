#include "ray.h"

Ray::Ray(const Point3& origin, const vec3& direction, double time) : origin(origin), direction(direction), tm(time) {}

Ray::Ray(const Point3& origin, const vec3& direction): Ray(origin, direction, 0) {}

const Point3 Ray::get_origin() const
{
    return origin;
}

const vec3 Ray::get_direction() const
{
    return direction;
}

Point3 Ray::at(double t) const{
    return (origin + t * direction);
}

