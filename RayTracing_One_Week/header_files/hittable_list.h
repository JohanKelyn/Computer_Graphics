#pragma once

#include "hittable.h"
#include "aabb.h"
#include <vector>

using std::make_shared;
using std::shared_ptr;

class Hittable_List : public Hittable {
  public:
    std::vector<std::shared_ptr<Hittable>> objects;

    Hittable_List();
    Hittable_List(std::shared_ptr<Hittable> object);

    void clear();
    void add(std::shared_ptr<Hittable> object);
    bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override;
    AABB bounding_box() const override;

  private:
    AABB bbox;
};