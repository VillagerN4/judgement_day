#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "map.hpp"

using namespace sf;

class Player {
public:
    Player();
    Player(float startX, float startY, Map worldMap);
    void tickPlayer(float deltaTime);
    void draw(RenderWindow& window, float displaySize, float cameraX, float cameraY, float tileSize, float deltaTime);

    bool checkCollision();
    
    float getX();
    float getY();

    void moveX(float amount);
    void moveY(float amount);

private:
    enum class action { idle, walk };

    action state;
    int facing;

    int frameTimer;
    int walkFrame;
    bool canFootstep;

    float acc;
    float res;

    float collisionBoxWidth;
    float collisionBoxHeight;

    float x;
    float y;

    float velocityX;
    float velocityY;

    Map worldMap;

    RectangleShape playerSprite;
    RectangleShape shadowSprite;
};
