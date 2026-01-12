#pragma once
#include "interval.h"
#include "ray.h"

class AABB {
  public:
    Interval x, y, z;

    AABB();

    AABB(const Interval& x, const Interval& y, const Interval& z);

    AABB(const Point3& a, const Point3& b);

    AABB(const AABB& box0, const AABB& box1);

    const Interval& axis_interval(int n) const;

    bool hit(const Ray& r, Interval ray_t) const;

    int longest_axis() const;

    static const AABB empty, universe;

  private:
    void pad_to_minimums();
};


