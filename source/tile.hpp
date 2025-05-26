#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class Tile
{
    public:
        int tilesetX;
        int tilesetY;
        Color loadColor;
        int soundGroup;
        int subtileCount;
        bool canBeOverlayed;
        bool canConnect;
        bool canCollide;
        int overlayX;
        int overlayY;
        int connectX;
        int connectY;
        vector<int> connections;
        int overlayId;

        Tile(int x, int y, Color loadColor, int soundGroup, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide);
        Tile(int x, int y, Color loadColor, int soundGroup, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int overlayX, int overlayY, int overlayId);
        Tile(int x, int y, Color loadColor, int soundGroup, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int overlayX, int overlayY, int connectX, int connectY, vector<int> connections, int overlayId);

        Tile();
};

#endif