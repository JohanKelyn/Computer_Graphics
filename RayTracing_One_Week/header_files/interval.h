#pragma once
#include<limits>
#include "utility.h"


class Interval {
    public:
    double min, max;

    Interval(); // Default interval is empty

    Interval(double min, double max);

    Interval(const Interval& a, const Interval& b);

    double size() const;

    bool contains(double x) const;

    bool surrounds(double x) const;

    double clamp(double x) const;

    Interval expand(double delta) const;

    static const Interval empty, universe;                          
};