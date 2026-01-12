#include "bvh.h"

BVH_Node::BVH_Node(Hittable_List list) : BVH_Node(list.objects, 0, list.objects.size()) {
    // There's a C++ subtlety here. This constructor (without span indices) creates an
    // implicit copy of the hittable list, which we will modify. The lifetime of the copied
    // list only extends until this constructor exits. That's OK, because we only need to
    // persist the resulting bounding volume hierarchy.
}

BVH_Node::BVH_Node(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end) {
    // Build the bounding box of the span of source objects.
    bbox = AABB::empty;
    for (size_t object_index=start; object_index < end; object_index++)
        bbox = AABB(bbox, objects[object_index]->bounding_box());

    int axis = bbox.longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                    : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        left = objects[start];
        right = objects[start+1];
    } else {
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

        auto mid = start + object_span/2;
        left = make_shared<BVH_Node>(objects, start, mid);
        right = make_shared<BVH_Node>(objects, mid, end);
    }

    bbox = AABB(left->bounding_box(), right->bounding_box());
}

bool BVH_Node::hit(const Ray& r, Interval ray_t, Hit_Record& rec) const {
    if (!bbox.hit(r, ray_t))
        return false;

    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
}

AABB BVH_Node::bounding_box() const { return bbox; }

bool BVH_Node::box_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis_index) {
    auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
}

bool BVH_Node::box_x_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 0);
}

bool BVH_Node::box_y_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 1);
}

bool BVH_Node::box_z_compare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
    return box_compare(a, b, 2);
}

