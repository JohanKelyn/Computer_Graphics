#include "hittable_list.h"

Hittable_List::Hittable_List() {}

Hittable_List::Hittable_List(std::shared_ptr<Hittable> object) {
    add(object);
}

void Hittable_List::clear() {
    objects.clear();
}

void Hittable_List::add(std::shared_ptr<Hittable> object)
{
    objects.push_back(object);
    bbox = AABB(bbox, object->bounding_box());
}

bool Hittable_List::hit(const Ray& r, Interval ray_t, Hit_Record& rec) const {
    Hit_Record temp_rec;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        if (object->hit(r, Interval(ray_t.min, closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

AABB Hittable_List::bounding_box() const  { return bbox; }

