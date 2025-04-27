#ifndef BOSSES
#define BOSSES

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class Boss {
public:
    RectangleShape shape;
    float speed;
    int hp;

    Boss(float x, float y, Color color, int hp = 10);

    void move(float offsetX);
};

class Projectile {
public:
    CircleShape shape;
    Vector2f velocity;

    Projectile(float x, float y, Vector2f vel);

    void update(float deltaTime);
};

class Crystal {
public:
    RectangleShape shape;
    int hp;
    bool alive;

    Crystal(float x, float y);

    void takeDamage(int dmg);
};

#endif
