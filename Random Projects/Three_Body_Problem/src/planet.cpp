#include "planet.h"

Planet::Planet(double m, double r, Vector2d pos) {
    mass = m;
    radius = r;
    position = pos;
    shape.setRadius(r);
    shape.setPosition(position[0]-r, position[1]-r);
    
}

void Planet::updatePosition(double dt) {
    position += dt * velocity;
    shape.setPosition(position[0]-radius, position[1]-radius);
} 

