#include "worldmap.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "map.hpp"
#include "player.hpp"

using namespace sf;
using namespace std;

Map world;
Player player;

const float worldDisplaySize = 4.f;
const float tileSize = 32.f;
const float displaySize = worldDisplaySize * tileSize;

float cameraX = 0.f;
float cameraY = 0.f;

float camLimitX = 0.f;
float camLimitY = 0.f;

void loadMap(const char* path){
    Texture tileset;
    if (!tileset.loadFromFile("assets\\textures\\tile\\tileset.png")){
        cout << "Tileset not found!" << endl;
    }

    world = Map(tileset, path, tileSize, worldDisplaySize);

    player = Player(0.5f, 10.5f, world);
}

void worldTick(float deltaTime){
    player.tickPlayer(deltaTime);
}

void displayWorld(RenderWindow& window, Vector2u window_size){
        cameraX = player.getX() * displaySize - window_size.x/2;
        cameraY = player.getY() * displaySize - window_size.y/2;

        camLimitX = (world.getMapWidth() * displaySize) - window_size.x;
        camLimitY = (world.getMapHeight() * displaySize) - window_size.y;

        if(cameraX < 0)
            cameraX = 0;
        if(cameraY < 0)
            cameraY = 0;
        if(cameraX > camLimitX)
            cameraX = camLimitX;
        if(cameraY > camLimitY)
            cameraY = camLimitY;

        world.setPosition(Vector2f(-cameraX, -cameraY));
        window.draw(world);
        player.draw(window, displaySize, cameraX, cameraY);
}