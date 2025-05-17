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

        Tile(int x, int y);
};

#endif