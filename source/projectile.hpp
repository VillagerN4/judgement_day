#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <SFML/Graphics.hpp>
using namespace sf;

class Projectile {
public:
    CircleShape shape;
    Vector2f velocity;

    Projectile(float x, float y, Vector2f vel);
    void update(float deltaTime);
    void draw(RenderWindow& window);
};

#endif
