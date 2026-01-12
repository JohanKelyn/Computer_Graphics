#pragma once

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class BVH_Node : public Hittable {
  public:
    BVH_Node(Hittable_List list);

    BVH_Node(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end);

    bool hit(const Ray& r, Interval ray_t, Hit_Record& rec) const override;

    AABB bounding_box() const override;

  private:
    shared_ptr<Hittable> left;
    shared_ptr<Hittable> right;
    AABB bbox;

    static bool box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis_index);

    static bool box_x_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b);

    static bool box_y_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b);

    static bool box_z_compare (const shared_ptr<Hittable> a, const shared_ptr<Hittable> b);
};