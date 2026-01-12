#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include<Eigen/Dense>

using Point3 = Eigen::Vector3d;
using vec3 = Eigen::Vector3d;
using Color = Eigen::Vector3d;

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // Returns a random real in [0,1).
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

static vec3 random_vector() {
    return vec3(random_double(), random_double(), random_double());
}

static vec3 random_vector(double min, double max) {
    return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}

inline vec3 random_unit_vector() {
    while (true) {
        auto p = random_vector(-1,1);
        auto lensq = p.squaredNorm();
        if (1e-160 < lensq && lensq <= 1)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (on_unit_sphere.dot(normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

inline bool near_zero(vec3 e) {
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*v.dot(n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = std::fmin((-uv).dot(n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.squaredNorm())) * n;
    return r_out_perp + r_out_parallel;
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.squaredNorm() < 1)
            return p;
    }
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return int(random_double(min, max+1));
}