#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

Tile::Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide){
    this->tilesetX = x;
    this->tilesetY = y;
    this->loadColor = loadColor;
    this->subtileCount = subtileCount;
    this->canBeOverlayed = canBeOverlayed;
    this->canConnect = canConnect;
    this->canCollide = canCollide;
}

Tile::Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int specialTilesetX, int specialTilesetY){
    this->tilesetX = x;
    this->tilesetY = y;
    this->loadColor = loadColor;
    this->subtileCount = subtileCount;
    this->canBeOverlayed = canBeOverlayed;
    this->canConnect = canConnect;
    this->canCollide = canCollide;
    this->specialTilesetX = specialTilesetX;
    this->specialTilesetY = specialTilesetY;
}

Tile::Tile(){
    
}