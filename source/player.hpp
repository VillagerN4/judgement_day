#pragma once

#include <SFML/Graphics.hpp>
#include "map.hpp"

using namespace sf;

class Player {
public:
    Player();
    Player(float startX, float startY, Map worldMap);
    void tickPlayer(float deltaTime);
    void draw(RenderWindow& window, float displaySize, float cameraX, float cameraY);

    float getX();
    float getY();

    void moveX(float amount);
    void moveY(float amount);

private:
    float acc;
    float res;

    float x;
    float y;

    float velocityX;
    float velocityY;

    Map worldMap;

    RectangleShape shape;
};
