#ifndef MAPGRID_H
#define MAPGRID_H

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "stb_image.h"
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

class Map : public Drawable
{
    private:
        Texture tileset;
        VertexArray vertexArray;
        
        int mapWidth;
        int mapHeight;
        float tileSize;
        float tileDisplaySize;

        void draw(RenderTarget& target, RenderStates states) const;

    public:

        Map(Texture tileset, int mapWidth, int mapHeight, float tileSize, float tileDisplaySize);
        
        Map(Texture tileset, const char* path, float tileSize, float tileDisplaySize);

        Map();
        
};

#endif