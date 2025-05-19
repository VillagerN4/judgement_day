#include "map.hpp"
#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <random>
#include <ctime>
#include <iostream>

using namespace sf;
using namespace std;

const int tilesetTileCount = 3;

Tile tilesetList[tilesetTileCount] = {
    Tile(0, 0, Color(0,0,0,0),0),
    Tile(1, 0, Color(51,118,54,0),3),
    Tile(6, 0, Color(149,146,144,0),3)
};

Map::Map(Texture tileset, int mapWidth, int mapHeight, float tileSize, float tileDisplaySize){
    this->tileset = tileset;
    this->mapWidth = mapWidth;
    this->mapHeight = mapHeight;
    this->tileSize = tileSize;
    this->tileDisplaySize = tileDisplaySize;

    vertexArray = VertexArray(PrimitiveType::Triangles, mapWidth * mapHeight * 6);

    int beginX = 1;

    Tile levelTile(beginX,0,Color::Black,3);

    for(int x = 0; x < mapWidth; x++){
        for(int y = 0; y < mapHeight; y++){
            srand(time(0) + (x + 1) * (y + 1) * this->mapWidth);

            levelTile.tilesetX = beginX + rand() % levelTile.subtileCount;

            const int tu = levelTile.tilesetX;
            const int tv = levelTile.tilesetY;

            Vertex* triangles = &vertexArray[(x + y * mapWidth) * 6];

            triangles[0].position = Vector2f(x * this->tileSize * this->tileDisplaySize, y * this->tileSize * this->tileDisplaySize);
            triangles[1].position = Vector2f((x + 1) * this->tileSize * this->tileDisplaySize, y * this->tileSize * this->tileDisplaySize);
            triangles[2].position = Vector2f(x * this->tileSize * this->tileDisplaySize, (y + 1) * this->tileSize * this->tileDisplaySize);
            triangles[3].position = Vector2f(x * this->tileSize * this->tileDisplaySize, (y + 1) * this->tileSize * this->tileDisplaySize);
            triangles[4].position = Vector2f((x + 1) * this->tileSize * this->tileDisplaySize, y * this->tileSize * this->tileDisplaySize);
            triangles[5].position = Vector2f((x + 1) * this->tileSize * this->tileDisplaySize, (y + 1) * this->tileSize * this->tileDisplaySize);

            triangles[0].texCoords = Vector2f(tu * this->tileSize, tv * this->tileSize);
            triangles[1].texCoords = Vector2f((tu + 1) * this->tileSize, tv * this->tileSize);
            triangles[2].texCoords = Vector2f(tu * this->tileSize, (tv + 1) * this->tileSize);
            triangles[3].texCoords = Vector2f(tu * this->tileSize, (tv + 1) * this->tileSize);
            triangles[4].texCoords = Vector2f((tu + 1) * this->tileSize, tv * this->tileSize);
            triangles[5].texCoords = Vector2f((tu + 1) * this->tileSize, (tv + 1) * this->tileSize);

        }
    }
}

Map::Map(){

}

void Map::draw(RenderTarget& target, RenderStates states) const{
    states.texture = &(this->tileset);

    target.draw(this->vertexArray, states);
}