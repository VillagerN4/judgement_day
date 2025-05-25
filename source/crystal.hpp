#ifndef CRYSTAL_HPP
#define CRYSTAL_HPP

#include <SFML/Graphics.hpp>
#include <string>
using namespace std;
using namespace sf;
class Crystal {
public:
    Texture texture;
    unique_ptr<sf::Sprite> sprite;
    Crystal(const string& texturePath, float cx, float cy, float radius, float startAngle = 0.f, float angularSpeed = 1.f);

    void update(float dt);
    void takeDamage(int dmg);
    void draw(RenderWindow& window);
    FloatRect getGlobalBounds() const;
    int getHP() const;
    bool isAlive() const;

private:
    void updatePosition();


    Vector2f center;
    float radius;
    float angle;
    float angularSpeed;
    bool alive;
    int hp;
};

#endif
