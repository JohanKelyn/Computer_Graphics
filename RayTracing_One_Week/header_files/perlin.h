#pragma once
#include "utility.h"

class Perlin {
  public:
    Perlin();

    double noise(const Point3& p) const;
    double turb(const Point3& p, int depth) const;

  private:
    static const int point_count = 256;
    vec3 randvec[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_perm(int* p);

    static void permute(int* p, int n);

    static double perlin_interp(const vec3 c[2][2][2], double u, double v, double w);
};