#ifndef BOSSES_HPP
#define BOSSES_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include "projectile.hpp"
#include "hero.hpp"
using namespace std;
using namespace sf;

class Boss {
public:
    RectangleShape shape;
    RectangleShape shotgunShape;
    Texture shotgunTexture;
    unique_ptr<Sprite> sprite;

    Boss(float x, float y, const string& texturePath, int HP);

    void takeDamage(int damage);
    void phaseTwoAttack(Hero& player, vector<Projectile>& projectiles);
    bool isAlive() const;
    void draw(RenderWindow& window);
    int getHP() const ;
private:
    Texture texture;
    int hp;
};

#endif
