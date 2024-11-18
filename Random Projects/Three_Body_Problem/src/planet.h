#ifndef PLANET_H
#define PLANET_H

#include<Eigen/Dense>
#include<SFML/Graphics.hpp>

using namespace Eigen;
using namespace std;

class Planet {
public:
    double mass;
    double radius;
    Vector2d position;
    Vector2d velocity = Vector2d(0.0, 0.0);
    sf::CircleShape shape;
    Planet(double mass, double radius, Vector2d position);
    void updatePosition(double dt);
};

#endif