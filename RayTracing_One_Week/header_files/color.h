#pragma once
#include<Eigen/Dense>
#include<iostream>
#include "interval.h"

inline double linear_to_gamma(double linear_component);
void write_color(std::ostream& out, const Color& pixel_color);