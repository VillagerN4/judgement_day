#include "Player.hpp"
#include "map.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

using namespace sf;

int keyX = 0;
int keyY = 0;

Player::Player(){
    
}

Player::Player(float startX, float startY, Map worldMap) {
    this->worldMap = worldMap;
    this->acc = 1.5f;
    this->res = 0.7f;
    this->x = startX;
    this->y = startY;

    this->shape.setFillColor(Color::Red);
}

float Player::getX(){
    return this->x;
}

float Player::getY(){
    return this->y;
}

void Player::moveX(float amount){
    int tile = this->worldMap.getTile(floor(this->x), floor(this->y));
    if(!(tile == -1 || this->worldMap.getCollision(tile))){
        float direction = amount / abs(amount);

        this->x += amount;

        tile = worldMap.getTile(floor(this->x), floor(this->y));

        while(tile == -1 || this->worldMap.getCollision(tile)){
            this->x += 0.001 * (-direction);
            tile = worldMap.getTile(floor(this->x), floor(this->y));
        }
    }
}

void Player::moveY(float amount){
    int tile = this->worldMap.getTile(floor(this->x), floor(this->y));
    if(!(tile == -1 || this->worldMap.getCollision(tile))){
        float direction = amount / abs(amount);

        this->y += amount;

        tile = this->worldMap.getTile(floor(this->x), floor(this->y));

        while(tile == -1 || worldMap.getCollision(tile)){
            this->y += 0.001 * (-direction);
            tile = worldMap.getTile(floor(this->x), floor(this->y));
        }
    }
}

void Player::tickPlayer(float deltaTime) {
    keyX =  (Keyboard::isKeyPressed(Keyboard::Scan::D) ? 1 : 0) - (Keyboard::isKeyPressed(Keyboard::Scan::A) ? 1 : 0);
    keyY =  (Keyboard::isKeyPressed(Keyboard::Scan::S) ? 1 : 0) - (Keyboard::isKeyPressed(Keyboard::Scan::W) ? 1 : 0);

    velocityX += keyX * this->acc * deltaTime;
    velocityY += keyY * this->acc * deltaTime;
    velocityX *= this->res;
    velocityY *= this->res;

    float totalVelocity = sqrt(velocityX * velocityX + velocityY * velocityY);

    if(totalVelocity > 0){
        moveX(velocityX * abs(velocityX / totalVelocity));
        moveY(velocityY * abs(velocityY / totalVelocity));
    }
}

void Player::draw(RenderWindow& window, float displaySize, float cameraX, float cameraY) {
    this->shape.setSize(Vector2f(1.f * displaySize, 1.f * displaySize));
    this->shape.setPosition(Vector2f((this->x - 0.5) * displaySize - cameraX, (this->y - 0.5) * displaySize - cameraY));
    window.draw(this->shape);
}
