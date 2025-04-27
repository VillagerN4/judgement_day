#include "Hero.hpp"

Hero::Hero(float x, float y, int HP) : hp(HP), sprite(texture) {
    if (!texture.loadFromFile("plik")) {
        shape.setSize({50.f, 80.f});
        shape.setFillColor(Color::Blue);
        shape.setPosition(Vector2f(x, y));
    } else {
        sprite.setPosition(Vector2f(x, y));
    }
}

void Hero::move(float speed, float deltaTime) {
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
        if (texture.loadFromFile("plik")) {
            sprite.move(Vector2f(-speed * deltaTime, 0));
        } else {
            shape.move(Vector2f(-speed * deltaTime, 0));
        }
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
        if (texture.loadFromFile("plik")) {
            sprite.move(Vector2f(speed * deltaTime, 0));
        } else {
            shape.move(Vector2f(speed * deltaTime, 0));
        }
    }
}

void Hero::attack() {}

void Hero::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
}

void Hero::draw(RenderWindow &window) {
    if (texture.loadFromFile("plik")) {
        window.draw(sprite);
    } else {
        window.draw(shape);
    }
}
