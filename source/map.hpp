#ifndef MAPGRID_H
#define MAPGRID_H

#include <SFML/Graphics.hpp>
#include "tile.hpp"
#include "stb_image.h"
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

class Map : public Drawable, public Transformable
{
    private:
        Texture tileset;
        VertexArray vertexArray;

        int mapWidth;
        int mapHeight;
        
        float tileSize;
        float tileDisplaySize;
        vector<int> tileList;

        void draw(RenderTarget& target, RenderStates states) const;

        void addVertex(int x, int y, int tu, int tv, int offset);

    public:

        // Map(Texture tileset, int mapWidth, int mapHeight, float tileSize, float tileDisplaySize);
        
        Map(Texture tileset, const char* path, float tileSize, float tileDisplaySize);

        Map();

        int getTile(int x, int y);

        int getMapWidth();
        int getMapHeight();

        bool getCollision(int tile);
        
};

#endif