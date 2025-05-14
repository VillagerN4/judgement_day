#ifndef HERO_HPP
#define HERO_HPP

#include <SFML/Graphics.hpp>
using namespace sf;
class Hero {
public:
    RectangleShape shape;
    int hp;
    bool isJumping;
    float jumpSpeed;

    Hero(float x, float y, int HP) : hp(HP), isJumping(false), jumpSpeed(0.f) {
        shape.setSize(Vector2f(50.f, 80.f));
        shape.setFillColor(Color::Blue);
        shape.setPosition(Vector2f(x, y));
    }

    void handleMovement(float deltaTime, float speed, Vector2u windowSize);
    void handleJumping(float deltaTime, float groundHeight);
    void takeDamage(int damage) { hp -= damage; }
    void draw(RenderWindow& window) { window.draw(shape); }
};

#endif
