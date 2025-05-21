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

Tile::Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int overlayX, int overlayY, int overlayId){
    this->tilesetX = x;
    this->tilesetY = y;
    this->loadColor = loadColor;
    this->subtileCount = subtileCount;
    this->canBeOverlayed = canBeOverlayed;
    this->canConnect = canConnect;
    this->canCollide = canCollide;
    this->overlayX = overlayX;
    this->overlayY = overlayY;
    this->overlayId = overlayId;
}

Tile::Tile(int x, int y, Color loadColor, int subtileCount, bool canBeOverlayed, bool canConnect, bool canCollide, int overlayX, int overlayY, int connectX, int connectY, vector<int> connections, int overlayId){
    this->tilesetX = x;
    this->tilesetY = y;
    this->loadColor = loadColor;
    this->subtileCount = subtileCount;
    this->canBeOverlayed = canBeOverlayed;
    this->canConnect = canConnect;
    this->canCollide = canCollide;
    this->overlayX = overlayX;
    this->overlayY = overlayY;
    this->connectX = connectX;
    this->connectY = connectY;
    this->connections = connections;
    this->overlayId = overlayId;
}

Tile::Tile(){
    
}