#ifndef BOSSES_HPP
#define BOSSES_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "projectile.hpp"
#include "hero.hpp"
using namespace sf;
using namespace std;
class Boss {
public:
    RectangleShape shape;
    int hp;

    Boss(float x, float y, Color color, int HP) : hp(HP) {
        shape.setSize({60.f, 100.f});
        shape.setPosition(Vector2f(x, y));
        shape.setFillColor(color);
    }

    void takeDamage(int damage) { hp -= damage; }
    void phaseTwoAttack(Hero& player, vector<Projectile>& projectiles);
};

#endif
