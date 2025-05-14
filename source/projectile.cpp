#include "projectile.hpp"
using namespace sf;
Projectile::Projectile(float x, float y, Vector2f vel) {
    shape.setRadius(10.f);
    shape.setFillColor(Color::Yellow);
    shape.setPosition(Vector2f(x, y));
    velocity = vel;
}

void Projectile::update(float deltaTime) {
    shape.move(velocity * deltaTime);
}

void Projectile::draw(RenderWindow& window) {
    window.draw(shape);
}
