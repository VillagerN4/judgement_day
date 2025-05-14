#ifndef CRYSTAL_HPP
#define CRYSTAL_HPP

#include <SFML/Graphics.hpp>
using namespace sf;
class Crystal {
public:
    RectangleShape shape;
    int hp;
    bool alive;

    Crystal(float x, float y) : hp(5), alive(true) {
        shape.setSize({30.f, 30.f});
        shape.setFillColor(Color::Cyan);
        shape.setPosition(Vector2f(x, y));
    }

    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp <= 0) alive = false;
    }

    void draw(RenderWindow& window) {
        if (alive) window.draw(shape);
    }
};

#endif
