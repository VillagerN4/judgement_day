#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

Tile::Tile(int x, int y, Color loadColor, int subtileCount){
    this->tilesetX = x;
    this->tilesetY = y;
    this->loadColor = loadColor;
    this->subtileCount = subtileCount;
}

Tile::Tile(){
    
}