#include "hero.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

Texture playerSheetUp;
Texture playerSheetLeg;

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
    dir = Vector2f(0.f, 0.f);
    if (!playerSheetUp.loadFromFile("assets\\textures\\entity\\player\\player_upbdy_64.png")){
    }
    if (!playerSheetLeg.loadFromFile("assets\\textures\\entity\\player\\player_legs_64.png")){
    }

    upper.setTexture(&playerSheetUp);
    legs.setTexture(&playerSheetLeg);

    shape.setSize(Vector2f(50.f, 80.f));
    upper.setSize(Vector2f(140.f, 140.f));
    legs.setSize(upper.getSize());
    shape.setFillColor(Color::Blue);
    shape.setPosition(Vector2f(x, y));
    legs.setPosition(Vector2f(x, y));
    upper.setPosition(Vector2f(x, y));

    shotgun.setSize(Vector2f(60.f, 20.f));
    shotgun.setFillColor(Color::Red);
    shotgun.setOrigin(Vector2f(0.f, 10.f));
    shotgun.setPosition(Vector2f(x + shape.getSize().x / 2.f, y + shape.getSize().y / 2.f));

    frameTimer = 0;
    walkTimer = 0;
    aim = 0;
    shootFrame = 0;
}

void Hero::handleMovement(float deltaTime, float speed, Vector2u windowSize) {
    if (Keyboard::isKeyPressed(Keyboard::Key::W)) {dir.y = -1.f;}else {dir.y = 0.f;}

    if(dir.y == -1.f){
        dir.x = (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::A)) ? (facing == 0 ? 1 : -1) : 0;
    
    }else{
        dir.x = (facing == 0 ? 1 : -1);
    }

    if (dir.x == 0.f && dir.y == 0.f) dir.x = 1.f;

    aim = 0;

    if(dir.x == 0.f && dir.y == -1.f)
        aim = 5;
    
    if(abs(dir.x) == 1.f && dir.y == -1.f)
        aim = 2;

    frameTimer += ceil(deltaTime);
    if(!isJumping)
        legsState = legsAction::idle;
    if (Keyboard::isKeyPressed(Keyboard::Key::A)){
        facing = 1;
        if(!isJumping && !(Keyboard::isKeyPressed(Keyboard::Key::D)))
            legsState = legsAction::walk;
        shape.move(Vector2f(-speed * deltaTime, 0.f));
    }
    if (Keyboard::isKeyPressed(Keyboard::Key::D)){
        facing = 0;
        if(!isJumping && !(Keyboard::isKeyPressed(Keyboard::Key::A)))
            legsState = legsAction::walk;
        shape.move(Vector2f(speed * deltaTime, 0.f));
    }
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
            legsState = legsAction::jump;
        }
    }
}

void Hero::shoot() {
    if (projectiles.size() + 3 > maxProjectiles) return;
    shootFrame = frameTimer;

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
    walkTimer = floor(frameTimer / 10);

    legs.setPosition(Vector2f(shape.getPosition().x - legs.getSize().x/2 + shape.getSize().x/2, shape.getPosition().y - legs.getSize().y/2 + shape.getSize().y/2));
    upper.setPosition(Vector2f(legs.getPosition().x, legs.getPosition().y + ((walkTimer % 2 == 0 && legsState == legsAction::walk) ? 1 : 0)*(upper.getSize().y / 64)));

    legs.setTextureRect(IntRect({0 + (legsState == legsAction::jump ? 320 : 0), facing*64}, {64, 64}));
    if(legsState == legsAction::walk)
        legs.setTextureRect(IntRect({64 + (walkTimer % 4) * 64, facing*64}, {64, 64}));

    upper.setTextureRect(IntRect({aim * 64, 128 +  facing*256 + ((frameTimer - shootFrame) < 3 ? 64 : 0)}, {64, 64}));

    window.draw(legs);
    window.draw(upper);
    for (auto& proj : projectiles)
        proj.draw(window);
}