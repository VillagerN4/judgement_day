// hero.hpp
#ifndef HERO_HPP
#define HERO_HPP

#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

class Projectile2 {
public:
    CircleShape shape;
    Vector2f velocity;
    bool active;

    Projectile2(float x, float y, Vector2f vel);
    void update(float deltaTime);
    void draw(RenderWindow& window);
};

class Hero {
public:
    enum class legsAction { idle, walk, jump };
    enum class upperAction { idle, shoot };

    RectangleShape shape;
    RectangleShape upper;
    RectangleShape legs;
    RectangleShape shotgun; 
    int hp;
    bool isJumping;
    float jumpSpeed;
    int facing;
    int walkTimer;
    int aim;
    int frameTimer;
    Vector2f dir;
    int shootFrame;

    legsAction legsState;
    upperAction upperState;

    vector<Projectile2> projectiles;
    const int maxProjectiles = 5;

    Hero(float x, float y, int HP);

    void handleMovement(float deltaTime, float speed, Vector2u windowSize);
    void handleJumping(float deltaTime, float groundHeight);
    void shoot();
    void updateProjectiles2(float deltaTime);
    void takeDamage(int damage);
    void draw(RenderWindow& window);
};

#endif
