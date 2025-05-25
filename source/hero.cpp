#include "hero.hpp"
#include <cmath>
#include <algorithm>

Projectile2::Projectile2(float x, float y, Vector2f vel) : velocity(vel), active(true) {
    shape.setRadius(5.f);
    shape.setFillColor(Color::Yellow);
    shape.setPosition(Vector2f(x, y));
}

void Projectile2::update(float deltaTime) {
    if (!active) return;
    shape.move(velocity * deltaTime);
    Vector2f pos = shape.getPosition();
    if (pos.x < 0 || pos.x > 1920 || pos.y < 0 || pos.y > 1080)
        active = false;
}

void Projectile2::draw(RenderWindow& window) {
    if (active)
        window.draw(shape);
}

Hero::Hero(float x, float y, int HP) : hp(HP), isJumping(false), jumpSpeed(0.f) {
    shape.setSize(Vector2f(50.f, 80.f));
    shape.setFillColor(Color::Blue);
    shape.setPosition(Vector2f(x, y));

    shotgun.setSize(Vector2f(60.f, 20.f));
    shotgun.setFillColor(Color::Red);
    shotgun.setOrigin(Vector2f(0.f, 10.f));
    shotgun.setPosition(Vector2f(x + shape.getSize().x / 2.f, y + shape.getSize().y / 2.f));
}

void Hero::handleMovement(float deltaTime, float speed, Vector2u windowSize) {
    if (Keyboard::isKeyPressed(Keyboard::Key::A))
        shape.move(Vector2f(-speed * deltaTime, 0.f));
    if (Keyboard::isKeyPressed(Keyboard::Key::D))
        shape.move(Vector2f(speed * deltaTime, 0.f));
    if (shape.getPosition().x < 0.f)
        shape.setPosition(Vector2f(0.f, shape.getPosition().y));
    if (shape.getPosition().x + shape.getSize().x > windowSize.x)
        shape.setPosition(Vector2f(windowSize.x - shape.getSize().x, shape.getPosition().y));

    shotgun.setPosition(Vector2f(shape.getPosition().x + shape.getSize().x / 2.f, shape.getPosition().y + shape.getSize().y / 2.f));
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

void Hero::shoot() {
    if (projectiles.size() + 3 > maxProjectiles) return;

    Vector2f dir(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) dir.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S)) dir.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)) dir.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D)) dir.x += 1.f;
    if (dir.x == 0.f && dir.y == 0.f) dir.x = 1.f;

    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    dir /= length;

    Vector2f startPos = shape.getPosition() + Vector2f(shape.getSize().x / 2.f, shape.getSize().y / 2.f);

    float baseAngle = atan2(dir.y, dir.x);
    float spread = 15.f * (3.14159f / 180.f);

    for (int i = -1; i <= 1; ++i) {
        float angle = baseAngle + i * spread;
        Vector2f velocity(cos(angle), sin(angle));
        velocity *= 700.f;
        projectiles.emplace_back(startPos.x, startPos.y, velocity);
    }

    float shotgunAngle = baseAngle * 180.f / 3.14159f;
    Angle angle = degrees(shotgunAngle);
    shotgun.setRotation(angle);
}

void Hero::updateProjectiles2(float deltaTime) {
    for (auto& proj : projectiles)
        proj.update(deltaTime);

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const Projectile2& p) { return !p.active; }), projectiles.end());
}

void Hero::takeDamage(int damage) {
    hp -= damage;
}

void Hero::draw(RenderWindow& window) {
    window.draw(shape);
    window.draw(shotgun);
    for (auto& proj : projectiles)
        proj.draw(window);
}