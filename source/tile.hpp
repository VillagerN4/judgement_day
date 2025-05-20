#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class Tile
{
    public:
        int tilesetX;
        int tilesetY;
        Color loadColor;
        int subtileCount;
        bool canBeOverlayed;
        bool canConnect;
        bool canCollide;
        int specialTilesetX;
        int specialTilesetY;
        vector<int> connections;

        Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide);
        Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int specialTilesetX, int specialTilesetY);
        Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int specialTilesetX, int specialTilesetY, vector<int> connections);

        Tile();
};

#endif