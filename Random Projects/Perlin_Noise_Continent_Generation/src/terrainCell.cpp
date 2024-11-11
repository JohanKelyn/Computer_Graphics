#include "terrainCell.h"

terrainCell::terrainCell() {
    altitude = 0.0f;
    shape.setSize(sf::Vector2f(1.0, 1.0));
}

void terrainCell::calcColor() {
    if(altitude < -0.5) shape.setFillColor(sf::Color(0,0,0));
    else if(altitude >= -0.5 && altitude < 0.0) {
        shape.setFillColor(sf::Color(0, 0, mapColor(altitude, 0, -0.5, 255, 0)));
    }
    else if(altitude >= 0.0 && altitude < 0.6) {
        shape.setFillColor(sf::Color(0, mapColor(altitude, 0, 0.60, 100, 200), 0));
    } 
    else if(altitude >= 0.6 && altitude < 2) {
        float col = mapColor(altitude, 0.6, 2, 100, 200);
        shape.setFillColor(sf::Color(col, col, col));
    }
}

float terrainCell::mapColor(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}