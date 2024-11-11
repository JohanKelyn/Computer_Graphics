#ifndef TERRAINCELL_H
#define TERRAINCELL_H

#include <SFML/Graphics.hpp>
#include <Eigen/Dense>

using namespace Eigen;

class terrainCell {
public:
    float altitude = 0.0f;
    sf::RectangleShape shape;
    
    terrainCell();
    
    void calcColor();

    float mapColor(float value, float fromLow, float fromHigh, float toLow, float toHigh);
};
#endif

