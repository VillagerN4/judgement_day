#include "npc.hpp"

using namespace sf;

Texture shadow2;

NPC::NPC(){

}

NPC::NPC(sf::Vector2f position, Texture* texture) {
    this->x = position.x;
    this->y = position.y;
    this->shape.setTexture(texture);

    this->shape.setPosition(position);
    
   
    if (!shadow2.loadFromFile("assets\\textures\\entity\\shadow.png")){
        cout << "Shadow not found!" << endl;
    }
    this->shadowSprite.setTexture(&shadow2);
}

bool NPC::checkCollision(float pX, float pY) {
    if(abs(pX - this->x) > 0.5f)
        return false;
    if(abs(pY - this->y) > 0.5f)
        return false;
    return true;
}

float NPC::getX(){
    return this->x;
}

float NPC::getY(){
    return this->y;
}

void NPC::draw(sf::RenderWindow& window, float displaySize, float cameraX, float cameraY, float tileSize, float deltaTime) {
    this->shape.setSize(Vector2f(1.f * displaySize, 1.f * displaySize));
    this->shadowSprite.setSize(Vector2f(0.5f * displaySize, 0.5f * displaySize));

    this->shape.setPosition(Vector2f((this->x - 0.5) * displaySize - cameraX, (this->y - 1 - 1/tileSize) * displaySize - cameraY));
    
    int shadowX = (this->x - 0.25f) * tileSize + 1;
    int shadowY = (this->y - 0.25f) * tileSize - 1;

    this->shadowSprite.setTextureRect(IntRect({3 * 16, 0}, {16, 16}));
    this->shadowSprite.setPosition(Vector2f((shadowX/tileSize) * displaySize - cameraX, (shadowY/tileSize) * displaySize - cameraY));

    window.draw(this->shadowSprite);
    window.draw(this->shape);
}
