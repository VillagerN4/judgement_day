#pragma once
#include <SFML/Graphics.hpp>
#include "player.hpp"

using namespace sf;

class NPC {
public:
    NPC();
    NPC(Vector2f position, Texture* texture);
    bool checkCollision(float pX, float pY);
    void draw(RenderWindow& window, float displaySize, float cameraX, float cameraY, float tileSize, float deltaTime);
    float getX();
    float getY();

private:
    Texture npcT;
    float x;
    float y;
    RectangleShape shape;
    RectangleShape shadowSprite;
};
