#include "Player.hpp"
#include "map.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <ctime>
#include <cmath>

using namespace sf;

Texture playerSheet;
Texture shadow;

SoundBuffer bufferG1("assets\\sounds\\tile\\grass1.wav");
SoundBuffer bufferG2("assets\\sounds\\tile\\grass2.wav");
SoundBuffer bufferG3("assets\\sounds\\tile\\grass3.wav");
SoundBuffer bufferG4("assets\\sounds\\tile\\grass4.wav");
SoundBuffer bufferG5("assets\\sounds\\tile\\grass5.wav");
SoundBuffer bufferG6("assets\\sounds\\tile\\grass6.wav");
vector<SoundBuffer> grassSound = {bufferG1, bufferG2, bufferG3, bufferG4, bufferG5, bufferG6};
SoundBuffer bufferS1("assets\\sounds\\tile\\stone1.wav");
SoundBuffer bufferS2("assets\\sounds\\tile\\stone2.wav");
SoundBuffer bufferS3("assets\\sounds\\tile\\stone3.wav");
SoundBuffer bufferS4("assets\\sounds\\tile\\stone4.wav");
SoundBuffer bufferS5("assets\\sounds\\tile\\stone5.wav");
SoundBuffer bufferS6("assets\\sounds\\tile\\stone6.wav");
vector<SoundBuffer> stoneSound = {bufferS1, bufferS2, bufferS3, bufferS4, bufferS5, bufferS6};

int keyX = 0;
int keyY = 0;

Player::Player(){
    this->frameTimer = 0;
    this->walkFrame = 0;
    this->velocityX = 0;
    this->velocityY = 0;
    this->facing = 0;
    this->acc = 0.8f;
    this->res = 10.f;
    this->collisionBoxWidth = 0.5f;
    this->collisionBoxHeight = 0.2f;
}

Player::Player(float startX, float startY, Map worldMap) {
    if (!playerSheet.loadFromFile("assets\\textures\\entity\\player\\player_32.png")){
        cout << "Player sheet not found!" << endl;
    }
    if (!shadow.loadFromFile("assets\\textures\\entity\\shadow.png")){
        cout << "Shadow not found!" << endl;
    }
    this->frameTimer = 0;
    this->walkFrame = 0;
    this->velocityX = 0;
    this->velocityY = 0;
    this->facing = 0;
    this->worldMap = worldMap;
    this->acc = 0.8f;
    this->res = 10.f;
    this->x = startX;
    this->y = startY;
    this->collisionBoxWidth = 0.5f;
    this->collisionBoxHeight = 0.2f;
    this->state = action::idle;

    this->playerSprite.setTexture(&playerSheet);
    this->shadowSprite.setTexture(&shadow);
    this->shadowSprite.setTextureRect(IntRect({48, 0}, {16, 16}));
}

bool Player::checkCollision(){
    int tile0 = this->worldMap.getTile(floor(this->x - this->collisionBoxWidth/2), floor(this->y - this->collisionBoxHeight));
    int tile1 = this->worldMap.getTile(floor(this->x + this->collisionBoxWidth/2), floor(this->y - this->collisionBoxHeight));
    int tile2 = this->worldMap.getTile(floor(this->x - this->collisionBoxWidth/2), floor(this->y));
    int tile3 = this->worldMap.getTile(floor(this->x + this->collisionBoxWidth/2), floor(this->y));

    return this->worldMap.getCollision(tile0) || this->worldMap.getCollision(tile1) || this->worldMap.getCollision(tile2) || this->worldMap.getCollision(tile3);
}

float Player::getX(){
    return this->x;
}

float Player::getY(){
    return this->y;
}

void Player::moveX(float amount){
    if(!checkCollision()){
        float direction = amount / abs(amount);

        this->x += amount;

        while(checkCollision()){
            this->velocityX = 0;    
            this->x += 0.001 * (-direction);
        }
    }
}

void Player::moveY(float amount){
    if(!checkCollision()){
        float direction = amount / abs(amount);

        this->y += amount;

        while(checkCollision()){
            this->velocityY = 0;
            this->y += 0.001 * (-direction);
        }
    }
}

void Player::tickPlayer(float deltaTime) {
    keyX =  (Keyboard::isKeyPressed(Keyboard::Scan::D) ? 1 : 0) - (Keyboard::isKeyPressed(Keyboard::Scan::A) ? 1 : 0);
    keyY =  (Keyboard::isKeyPressed(Keyboard::Scan::S) ? 1 : 0) - (Keyboard::isKeyPressed(Keyboard::Scan::W) ? 1 : 0);

    velocityX += keyX * this->acc * deltaTime;
    velocityY += keyY * this->acc * deltaTime;
    velocityX /= 1 + (this->res * deltaTime);
    velocityY /= 1 + (this->res * deltaTime);

    float totalVelocity = sqrt(velocityX * velocityX + velocityY * velocityY);

    this->frameTimer += ceil((totalVelocity) * deltaTime);

    if(totalVelocity > 0){
        if(abs(velocityX) >= abs(velocityY)/2){
            this->facing = velocityX > 0 ? 2 : 3;
        }else{
            this->facing = velocityY > 0 ? 0 : 1;
        }
        moveX(velocityX * abs(velocityX / totalVelocity));
        moveY(velocityY * abs(velocityY / totalVelocity));

        this->state = action::walk;
    }
    if(totalVelocity < 0.01){
        this->state = action::idle;
    }
    if(abs(this->velocityX) < 0.01)
        this->velocityX = 0;
    if(abs(this->velocityY) < 0.01)
        this->velocityY = 0;
}

void Player::draw(RenderWindow& window, float displaySize, float cameraX, float cameraY, float tileSize, float deltaTime) {
    this->playerSprite.setSize(Vector2f(1.f * displaySize, 1.f * displaySize));
    this->shadowSprite.setSize(Vector2f(0.5f * displaySize, 0.5f * displaySize));
    this->playerSprite.setPosition(Vector2f((this->x - 0.5) * displaySize - cameraX, (this->y - 1 - 1/tileSize) * displaySize - cameraY));

    int shadowX = (this->x - 0.25f) * tileSize + 1;
    int shadowY = (this->y - 0.25f) * tileSize - 1;

    for(int offset = 0; offset < 4; offset++){
        this->shadowSprite.setTextureRect(IntRect({offset * 16, 0}, {16, 16}));
        if(worldMap.getCollision(worldMap.getTile(floor(this->x), floor((this->y + offset/tileSize)))))
            break;
    }
    if(this->state == action::idle)
        this->playerSprite.setTextureRect(IntRect({0, 32 * this->facing}, {32, 32}));
    this->walkFrame = ceil(this->frameTimer / 10);
    this->canFootstep = this->walkFrame % 2 == 0;
    if(canFootstep && this->state == action::walk){
        int soundGroup = worldMap.getSound(worldMap.getTile(floor(this->x), floor(this->y)));
        if(soundGroup == 0){
            srand(time(0) + frameTimer * 100);
            SoundBuffer buffer = stoneSound[0 + rand() % stoneSound.size()];
            Sound footstep(buffer);
            footstep.setVolume(100.f);
            footstep.play();
        } 
        canFootstep = false;
    }
    if(this->state == action::walk)
        this->playerSprite.setTextureRect(IntRect({32 + (this->walkFrame % 4) * 32, 32 * this->facing}, {32, 32}));

    this->shadowSprite.setPosition(Vector2f((shadowX/tileSize) * displaySize - cameraX, (shadowY/tileSize) * displaySize - cameraY));

    window.draw(this->shadowSprite);
    window.draw(this->playerSprite);
}
