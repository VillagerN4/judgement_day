#include "map.hpp"
#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

using namespace sf;
using namespace std;

Map::Map(Texture tileset, int mapWidth, int mapHeight, float tileSize, float tileDisplaySize){
    this->tileset = tileset;
    this->mapWidth = mapWidth;
    this->mapHeight = mapHeight;
    this->tileSize = tileSize;
    this->tileDisplaySize = tileDisplaySize;

    vertexArray = VertexArray(PrimitiveType::Triangles, mapWidth * mapHeight * 6);

    Tile test(0,0);

    for(int x = 0; x < mapWidth; x++){
        for(int y = 0; y < mapHeight; y++){

            const int tu = test.tilesetX;
            const int tv = test.tilesetY;

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