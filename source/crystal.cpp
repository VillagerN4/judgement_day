#include "crystal.hpp"
#include <cmath>
#include <iostream>
using namespace std;
using namespace sf;
Crystal::Crystal(const string& texturePath, float cx, float cy, float radius, float startAngle, float angularSpeed)
    : hp(5), alive(true), center(cx, cy), radius(radius), angle(startAngle), angularSpeed(angularSpeed)
{
    if (!texture.loadFromFile(texturePath)) {
        cout << "Nie udało się wczytać z pliku!";
    }
    sprite = make_unique<Sprite>(texture);
    sprite->setScale(Vector2f(2.f,2.f));
    sprite->setOrigin(Vector2f(texture.getSize().x / 2.f, texture.getSize().y / 2.f));
    sprite->setPosition(Vector2f(center.x + cos(angle) * radius, center.y + sin(angle) * radius));
}

void Crystal::update(float dt) {
    if (!alive) return;

    angle += angularSpeed * dt;

    float x = center.x + cos(angle) * radius;
    float y = center.y + sin(angle) * radius;

    sprite->setPosition(Vector2f(x, y));
}

void Crystal::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) alive = false;
}

void Crystal::draw(RenderWindow& window) {
    if (alive) window.draw(*sprite);
}
bool Crystal::isAlive() const {
    if(alive){
        return true;
    }else{
        return false;
    }
}
FloatRect Crystal::getGlobalBounds() const {
    if (!alive) return FloatRect(); 
    return sprite->getGlobalBounds();
}

int Crystal::getHP() const {
    return hp;
}