#include <iostream>
#include <ctime>

#include "terrainCell.h"
#define DB_PERLIN_IMPL
#include "db_perlin.h"

using namespace std;

int main() {
    srand(time(0));

    int width = 800;
    int height = 800;
    int scale = 1;
    float noiseScale = 0.0090*scale;
    int mapHeight = height/scale;
    int mapWidth = width/scale;
    sf::RenderWindow window(sf::VideoMode(width, height), "Projetc 1 - Perlin Noise Continent Generation");
    
    
   vector<sf::RectangleShape> terrain;
   float d, maxd, dx, dy;
   maxd = sqrt(mapWidth * mapWidth * 0.25 + mapHeight * mapHeight * 0.25);
   terrainCell cell;
    for(int y = 0; y < mapHeight; y++) {
        for(int x = 0; x < mapWidth; x++) {
            dx = (float)mapWidth * 0.5 - x;
            dy = (float) mapHeight * 0.5 - y;
            d = sqrt(dx * dx +  dy * dy);
            cell.altitude = 0.75f - 2 * d/maxd;
            cell.altitude += 2 * (float)db::perlin((float)x*noiseScale, (float)y*noiseScale) - 0.3;
            cell.shape.setPosition((float)x * (float) scale, (float)y * (float) scale);
            cell.shape.setSize(sf::Vector2f((float) scale, (float) scale));
            cell.calcColor();
            terrain.push_back(cell.shape);
        }
    }
    
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        for(auto cell: terrain) {
            window.draw(cell);
        }
        window.display();
    }
    

    return 0;
}