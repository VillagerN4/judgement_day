#include "worldmap.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "map.hpp"
#include "player.hpp"
#include "npc.hpp"

using namespace sf;
using namespace std;

Texture euf;

Texture blak;

Map world;
Player playerM;
NPC eufemia;
NPC npc2;

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
    if(!euf.loadFromFile("assets\\textures\\entity\\boss\\eufemia\\eufemia_32.png")){
        cout << "Texture not found!" << endl;
    }
    if(!blak.loadFromFile("assets\\textures\\entity\\boss\\black_guy.png")){
        cout << "Texture not found!" << endl;
    }

    world = Map(tileset, path, tileSize, worldDisplaySize);

    playerM = Player(0.5f, 10.f, world);

    eufemia = NPC(Vector2f{60.5f, 33.f}, &euf);
    npc2 = NPC(Vector2f{9.5f, 6.f}, &blak);
}

void worldTick(float deltaTime){
    playerM.tickPlayer(deltaTime);
}

bool checkEufemia(){
    return Keyboard::isKeyPressed(Keyboard::Scan::Enter) && eufemia.checkCollision(playerM.getX(), playerM.getY());
}

bool checkBlack(){
    return Keyboard::isKeyPressed(Keyboard::Scan::Enter) && npc2.checkCollision(playerM.getX(), playerM.getY());
}

void displayWorld(RenderWindow& window, Vector2u window_size, float deltaTime){
        cameraX = playerM.getX() * displaySize - window_size.x/2;
        cameraY = playerM.getY() * displaySize - window_size.y/2;

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
        if(playerM.getY() > eufemia.getY()){
            eufemia.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
                
            if(playerM.getY() > npc2.getY()){
                npc2.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
                playerM.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
            }else{
                playerM.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
                npc2.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
            }
        }else{
            if(playerM.getY() > npc2.getY()){
                npc2.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
                playerM.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
            }else{
                playerM.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
                
                npc2.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
            }
            eufemia.draw(window, displaySize, cameraX, cameraY, tileSize, deltaTime);
        }
}