#ifndef HERO
#define HERO

#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class Hero {
private:
    Texture texture;
    Sprite sprite;
public:
    RectangleShape shape;
    Vector2f velocity;
    float speed;
    int hp;
    string weapon;

    Hero(float x, float y, int HP = 100);

    void move(float speed, float deltaTime);
    void attack();
    void takeDamage(int damage);
    void draw(RenderWindow &window);
};

#endif
