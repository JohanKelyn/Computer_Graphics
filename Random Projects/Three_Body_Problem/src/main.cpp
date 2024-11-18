#include<iostream>

#include "planet.h"

void calculateForce(Planet &p1, Planet &p2, Planet &p3, double dt);
double dt = 0.0;

int main() {
    cout << "******************************" << endl;
    cout << "Project 2 - Three Body Problem" << endl;
    std::cout << "******************************" << endl;

    int width = 800;
    int height = 800;
    sf::RenderWindow window(sf::VideoMode(width, height), "Projetc 2 - Three Body Problem");

    Planet p1(1.0, 10.0, Vector2d(400.0, 400.0));
    p1.shape.setFillColor(sf::Color(255, 255, 0));
    p1.velocity = Vector2d(0.0, 0.0);
    Planet p2(1.0, 10.0, Vector2d(250.0, 250.0));
    p2.velocity = Vector2d(25, 1.0);
    p2.shape.setFillColor(sf::Color(255, 0, 0));
    Planet p3(1.0, 10.0, Vector2d(550.0, 550.0));
    p3.velocity = Vector2d(-25, -1.0);
    p3.shape.setFillColor(sf::Color(0, 255, 0));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        calculateForce(p1, p2, p3, dt);
        p1.updatePosition(dt);
        p2.updatePosition(dt);
        p3.updatePosition(dt);
        window.draw(p1.shape);
        window.draw(p2.shape);
        window.draw(p3.shape);
        dt = 0.01;
        window.display();
        
    }

    return 0;
}

void calculateForce(Planet &p1, Planet &p2, Planet &p3, double dt) {
    Vector2d d12 = (p2.position - p1.position).normalized();
    double dist12 = d12.norm();
    double force12 = 1 * p1.mass * p2.mass / (dist12 * dist12);

    Vector2d d13 = (p3.position - p1.position).normalized();
    double dist13 = d13.norm();
    double force13 = 1 * p1.mass * p3.mass / (dist13 * dist13);

    Vector2d d23 = (p3.position - p2.position).normalized();
    double dist23 = d23.norm();
    double force23 = 1 * p2.mass * p3.mass / (dist23 * dist23);

    Vector2d F12 = Vector2d(force12 * d12[0] / dist12, force12 * d12[1] / dist12);
    Vector2d F13 = Vector2d(force13 * d13[0] / dist13, force13 * d13[1] / dist13);
    Vector2d F23 = Vector2d(force23 * d23[0] / dist23, force23 * d23[1] / dist23);

    p1.velocity += ((F12 + F13) / p1.mass) * dt;
    p2.velocity += ((-F12 + F23) / p2.mass) * dt;
    p3.velocity += ((-F13 - F23) / p3.mass) * dt;
}