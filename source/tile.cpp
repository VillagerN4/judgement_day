#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace sf;
using namespace std;

Tile::Tile(int x, int y){
    this->tilesetX = x;
    this->tilesetY = y;
}