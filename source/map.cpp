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

int* tileList;

Map::Map(Texture tileset, const char* path, float tileSize, float tileDisplaySize){
    ifstream level;
    level.open(path, ios::in | ios::binary);

    if(!level.is_open()){
        cout << "Failed to open level file." << endl;
        return;
    }

    const int levelHeaderSize = 14;
    const int informationHeaderSize = 40;

    unsigned char levelHeader[levelHeaderSize];
    level.read(reinterpret_cast<char*>(levelHeader), levelHeaderSize);

    if(levelHeader[0] != 'B' || levelHeader[1] != 'M'){
        cout << "Attached file is not a bitmap." << endl;
        level.close();
        return;
    }

    unsigned char informationHeader[informationHeaderSize];
    level.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

    int levelFileSize = levelHeader[2] + (levelHeader[3] << 8) + (levelHeader[4] << 16) + (levelHeader[5] << 24);
    this->mapWidth = informationHeader[4] + (informationHeader[5] << 8) + (informationHeader[6] << 16) + (informationHeader[7] << 24);
    this->mapHeight = informationHeader[8] + (informationHeader[9] << 8) + (informationHeader[10] << 16) + (informationHeader[11] << 24);

    tileList = new int[mapWidth * mapHeight];

    const int paddingAmount = ((4 - (this->mapWidth * 3) % 4) % 4);

    for(int y = 0; y < this->mapHeight; y++){
        for(int x = 0; x < this->mapWidth; x++){
            unsigned char color[3];
            level.read(reinterpret_cast<char*>(color), 3);

            int tileIndex = 0;

            int r, g, b;
            r = static_cast<int>(color[2]);
            g = static_cast<int>(color[1]);
            b = static_cast<int>(color[0]);

            for(int i = 0; i < tilesetTileCount; i++){
                if(Color(r,g,b,0) == tilesetList[i].loadColor){
                    tileIndex = i;
                    break;
                }
            }

            // if(tileIndex < 1)
            // cout << "Tile " << x + y * this->mapWidth << ": " << tileIndex << endl;

            tileList[x + y * this->mapWidth] = tileIndex;
        }
        level.ignore(paddingAmount);
    }

    level.close();


    for(int x = 0; x < this->mapWidth; x++){    
        for(int y = 0; y < this->mapHeight/2; y++){
            int tmp = tileList[x + (this->mapHeight - y - 1) * this->mapWidth];
            tileList[x + (this->mapHeight - y - 1) * this->mapWidth] = tileList[x + y * this->mapWidth];
            tileList[x + y * this->mapWidth] = tmp;
        }
    }
    
    this->tileset = tileset;
    this->tileSize = tileSize;
    this->tileDisplaySize = tileDisplaySize;

    vertexArray = VertexArray(PrimitiveType::Triangles, this->mapWidth * this->mapHeight  * 6);

    Tile levelTile;

    for(int x = 0; x < mapWidth; x++){
        for(int y = 0; y < mapHeight; y++){
            srand(time(0) + (x + 1) * (y + 1) * this->mapWidth);

            levelTile = tilesetList[tileList[x + (y * this->mapWidth)]];
            
            if(levelTile.subtileCount > 0)
            levelTile.tilesetX += rand() % levelTile.subtileCount;

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