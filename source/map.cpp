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

const int overlayLayers = 4;
const int connectLayers = 4;

Tile emptyTile(0,1,Color(255,255,255,0),0,false,false,false);

Tile tilesetList[tilesetTileCount] = {
    Tile(0, 0, Color(0,0,0,0), 0, true, false, false),
    Tile(1, 0, Color(51,118,54,0), 5, false, false, false, 1, 1),
    Tile(6, 0, Color(149,146,144,0), 3, true, true, false, 5, 1)
};

const int overlayTileCount = 1;

int overlayTiles[overlayTileCount] = {1};

int* tileList;

int Map::getTile(int x, int y){
    if(x < 0 || x >= this->mapWidth){
        return 0;
    }
    if(y < 0 || y >= this->mapHeight){
        return 0;
    }
    return tileList[x + (y * this->mapWidth)];
}

bool Map::getCollision(int tile){
    return tilesetList[tile].canCollide;
}

void Map::addVertex(int x, int y, int tu, int tv, int offset){

    Vertex* triangles = &vertexArray[offset + ((x + y * mapWidth) * 6)];

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

    vertexArray = VertexArray(PrimitiveType::Triangles, this->mapWidth * this->mapHeight  * 6  * (1 + overlayLayers + connectLayers));

    Tile levelTile;

    int tu, tv;

    for(int x = 0; x < this->mapWidth; x++){
        for(int y = 0; y < this->mapHeight; y++){
            int seed = time(0) + (x + 1) * (y + 1) * this->mapWidth;
            srand(seed);

            levelTile = tilesetList[getTile(x, y)];
            
            if(levelTile.subtileCount > 0 && rand() % levelTile.subtileCount >= levelTile.subtileCount * 3/4){
                srand(seed * (rand() % 1000 + 1));
                levelTile.tilesetX += rand() % levelTile.subtileCount;
            }

            tu = levelTile.tilesetX;
            tv = levelTile.tilesetY;

            addVertex(x, y, tu, tv, 0);
        }
    }

    int connectOffset = this->mapWidth * this->mapHeight  * 6;
    int overlayOffset = this->mapWidth * this->mapHeight  * 6 * (1 + connectLayers);
    int neighbourTiles[4];
    int currentTile;

    for(int x = 0; x < this->mapWidth; x++){
        for(int y = 0; y < this->mapHeight; y++){
            currentTile = getTile(x, y);
            
            if(tilesetList[currentTile].canBeOverlayed){
                neighbourTiles[0] = getTile(x - 1, y);
                neighbourTiles[1] = getTile(x, y - 1);
                neighbourTiles[2] = getTile(x + 1, y);
                neighbourTiles[3] = getTile(x, y + 1);
                for(int n = 0; n < 4; n++){
                    for(int ol = 0; ol < overlayTileCount; ol++){
                        if(neighbourTiles[n] == overlayTiles[ol]){
                            tu = tilesetList[overlayTiles[ol]].specialTilesetX + n;
                            tv = tilesetList[overlayTiles[ol]].specialTilesetY;
                            addVertex(x, y, tu, tv, overlayOffset + (this->mapWidth * this->mapHeight * 6) * n);
                        }
                    }
                }
            }

            if(tilesetList[currentTile].canConnect){
                neighbourTiles[0] = getTile(x - 1, y);
                neighbourTiles[1] = getTile(x, y - 1);
                neighbourTiles[2] = getTile(x + 1, y);
                neighbourTiles[3] = getTile(x, y + 1);
                for(int n = 0; n < 4; n++){
                    if(neighbourTiles[n] != currentTile){
                        tu = tilesetList[currentTile].specialTilesetX + n;
                        tv = tilesetList[currentTile].specialTilesetY;
                        addVertex(x, y, tu, tv, connectOffset + (this->mapWidth * this->mapHeight * 6) * n);
                    }
                }
            }

        }
    }
}

Map::Map(){

}

void Map::draw(RenderTarget& target, RenderStates states) const{
    states.transform *= getTransform();

    states.texture = &(this->tileset);

    target.draw(this->vertexArray, states);
}