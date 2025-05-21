#include "map.hpp"
#include "tile.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <random>
#include <ctime>
#include <iostream>

using namespace sf;
using namespace std;

const int tilesetTileCount = 12;

const int overlayLayers = 8;
const int connectLayers = 8;

vector<int> pavementConnect = {2, 3, 4, 7, -1};
vector<int> redBrickConnect = {3, 4, 5, 6, -1};
vector<int> redBrickRoofConnect = {7, 8, -1};

const int overlayCount = 2;

int overlays[overlayCount][2] = {{0,1},{0,5}};

Tile tilesetList[tilesetTileCount] = {
    Tile(0, 0, Color(0,0,0,255), 0, false, false, false),
    Tile(1, 0, Color(51,118,54,0), 5, true, false, false, overlays[0][0], overlays[0][1], 0),
    Tile(6, 0, Color(149,146,144,0), 3, true, true, false, 0, 0, 12, 1, pavementConnect, -1),
    Tile(9, 0, Color(153,84,47,0), 3, true, true, true, overlays[1][0], overlays[1][1], 0, 2, redBrickConnect, 1),
    Tile(12, 0, Color(178,109,79,0), 2, false, true, true, overlays[1][0], overlays[1][1], 0, 2, redBrickConnect, 1),
    Tile(14, 0, Color(62,45,38,0), 4, false, false, true, overlays[1][0], overlays[1][1], 1),
    Tile(18, 0, Color(139,57,32,0), 0, false, false, true, overlays[1][0], overlays[1][1], 1),
    Tile(19, 0, Color(99,90,85,0), 2, false, true, true, overlays[1][0], overlays[1][1], 12, 2, redBrickRoofConnect, 1),
    Tile(21, 0, Color(141,78,37,0), 2, false, true, true, overlays[1][0], overlays[1][1], 12, 2, redBrickRoofConnect, 1),
    Tile(0, 3, Color(22,23,26,0), 0, true, true, false, 0, 0, 12, 4, {9, 10, 11, -1}, -1),
    Tile(1, 3, Color(185,191,211,0), 0, true, true, false, 0, 0, 0, 4, {10, -1}, -1),
    Tile(2, 3, Color(214,217,224,0), 0, true, true, false, 0, 0, 12, 4, {9, 10, 11, -1}, -1)
};

int Map::getTile(int x, int y){
    if(x < 0 || x >= this->mapWidth){
        return -1;
    }
    if(y < 0 || y >= this->mapHeight){
        return -1;
    }
    return this->tileList[x + (y * this->mapWidth)];
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

            this->tileList.push_back(tileIndex);
        }
        level.ignore(paddingAmount);
    }

    level.close();


    for(int x = 0; x < this->mapWidth; x++){    
        for(int y = 0; y < this->mapHeight/2; y++){
            int tmp = this->tileList[x + (this->mapHeight - y - 1) * this->mapWidth];
            this->tileList[x + (this->mapHeight - y - 1) * this->mapWidth] = this->tileList[x + y * this->mapWidth];
            this->tileList[x + y * this->mapWidth] = tmp;
        }
    }
    
    this->tileset = tileset;
    this->tileSize = tileSize;
    this->tileDisplaySize = tileDisplaySize;

    vertexArray = VertexArray(PrimitiveType::Triangles, this->mapWidth * this->mapHeight  * 6  * (1 + overlayLayers * overlayCount + connectLayers));

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
    int neighbourTiles[8];
    int currentTile;
    int detailOffset = 0;

    for(int x = 0; x < this->mapWidth; x++){
        for(int y = 0; y < this->mapHeight; y++){
            currentTile = getTile(x, y);
            detailOffset = 0;
            
            if(tilesetList[currentTile].canBeOverlayed){
                neighbourTiles[0] = getTile(x - 1, y);
                neighbourTiles[1] = getTile(x + 1, y);
                neighbourTiles[2] = getTile(x, y - 1);
                neighbourTiles[3] = getTile(x, y + 1);
                neighbourTiles[4] = getTile(x - 1, y + 1);
                neighbourTiles[5] = getTile(x + 1, y - 1);
                neighbourTiles[6] = getTile(x - 1, y - 1);
                neighbourTiles[7] = getTile(x + 1, y + 1);

                for(int n = 0; n < 8; n++){
                    detailOffset = 0;
                    for(int ol = 0; ol < overlayCount; ol++){
                        detailOffset = 0;
                        if(neighbourTiles[n] >= 0 && tilesetList[neighbourTiles[n]].overlayId >= 0 && tilesetList[neighbourTiles[n]].overlayId == ol){     
                            if(tilesetList[currentTile].overlayId >= 0){
                                if(tilesetList[currentTile].overlayId >= tilesetList[neighbourTiles[n]].overlayId)
                                    continue;
                            }
                            if(n >= 4){
                                if(tilesetList[neighbourTiles[n < 6 ? (7 - n) : (n - 6)]].overlayId == ol && tilesetList[neighbourTiles[n - 4]].overlayId == ol){
                                    detailOffset = 4;
                                }
                                if(detailOffset == 0){
                                    if(tilesetList[neighbourTiles[n < 6 ? (7 - n) : (n - 6)]].overlayId == ol || tilesetList[neighbourTiles[n - 4]].overlayId == ol)
                                        detailOffset = -1;
                                }
                            }

                            if(detailOffset >= 0){
                                tu = overlays[ol][0] + n + detailOffset;
                                tv = overlays[ol][1];
                                addVertex(x, y, tu, tv, overlayOffset + (this->mapWidth * this->mapHeight * 6) * (n + overlayLayers * ol));
                            }
                        }
                    }
                }
            }

            if(tilesetList[currentTile].canConnect){
                neighbourTiles[0] = getTile(x - 1, y);
                neighbourTiles[1] = getTile(x + 1, y);
                neighbourTiles[2] = getTile(x, y - 1);
                neighbourTiles[3] = getTile(x, y + 1);
                neighbourTiles[4] = getTile(x - 1, y + 1);
                neighbourTiles[5] = getTile(x + 1, y - 1);
                neighbourTiles[6] = getTile(x - 1, y - 1);
                neighbourTiles[7] = getTile(x + 1, y + 1);

                for(int n = 0; n < 8; n++){
                    detailOffset = 0;
                    for(int c = 0; c < tilesetList[currentTile].connections.size(); c++){
                        if(neighbourTiles[n] == tilesetList[currentTile].connections[c]){
                            neighbourTiles[n] = currentTile;
                            break;
                        }
                    }
                    if(neighbourTiles[n] != currentTile){
                        if(n >= 4){
                            if(neighbourTiles[n < 6 ? (7 - n) : (n - 6)] != currentTile && neighbourTiles[n - 4] != currentTile){
                                detailOffset = 4;
                            }
                            if(detailOffset == 0){
                                if(neighbourTiles[n < 6 ? (7 - n) : (n - 6)] != currentTile || neighbourTiles[n - 4] != currentTile)
                                    detailOffset = -1;
                            }
                        }
                    }else{
                        detailOffset = -1;
                    }


                    if(detailOffset >= 0){
                        tu = tilesetList[currentTile].connectX + n + detailOffset;
                        tv = tilesetList[currentTile].connectY;
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