#include "hero.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
using namespace sf;
void Hero::handleMovement(float deltaTime, float speed, Vector2u windowSize) {
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) shape.move(Vector2f(-speed * deltaTime, 0.f));
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) shape.move(Vector2f(speed * deltaTime, 0.f));
    
    if (shape.getPosition().x < 0.f) shape.setPosition(Vector2f( 0.f, shape.getPosition().y));
    if (shape.getPosition().x + shape.getSize().x > windowSize.x)
        shape.setPosition(Vector2f(windowSize.x - shape.getSize().x, shape.getPosition().y));
}

void Hero::handleJumping(float deltaTime, float groundHeight) {
    if (isJumping) {
        shape.move(Vector2f(0.f, jumpSpeed * deltaTime));
        jumpSpeed += 980.f * deltaTime;
        if (shape.getPosition().y >= groundHeight) {
            shape.setPosition(Vector2f(shape.getPosition().x, groundHeight));
            isJumping = false;
            jumpSpeed = 0.f;
        }
    } else {
        if (Keyboard::isKeyPressed(Keyboard::Key::Space)) {
            jumpSpeed = -sqrt(2.f * 980.f * 300.f);
            isJumping = true;
        }
    }
}
