#include "Bosses.hpp"

Boss::Boss(float x, float y, Color color, int hp) : hp(hp) {
    shape.setSize({50.f, 80.f});
    shape.setPosition(Vector2f(x, y));
    shape.setFillColor(color);
    speed = 200.f;
}

void Boss::move(float offsetX) {
    shape.move({offsetX, 0.f});
}

Projectile::Projectile(float x, float y, Vector2f vel) : velocity(vel) {
    shape.setRadius(40.f);
    shape.setFillColor(Color::Magenta);
    shape.setPosition({x, y});
}

void Projectile::update(float deltaTime) {
    shape.move(velocity * deltaTime);
}

Crystal::Crystal(float x, float y) {
    shape.setSize({30.f, 30.f});
    shape.setFillColor(Color::Cyan);
    shape.setPosition({x, y});
    hp = 5;
    alive = true;
}

void Crystal::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) {
        alive = false;
    }
}
