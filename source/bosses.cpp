#include "bosses.hpp"
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

Boss::Boss(float x, float y, const string& texturePath, int HP) : hp(HP) {
    if (!texture.loadFromFile(texturePath)) {
        cout << "Nie ma tekstury!" << endl;
        return;
    }

    sprite = make_unique<Sprite>(texture);
    sprite->setScale(Vector2f(2.5f,2.5f));
    sprite->setPosition(Vector2f(x, y));
}

void Boss::takeDamage(int damage) {
    hp -= damage;
}

bool Boss::isAlive() const {
    return hp > 0;
}

void Boss::draw(RenderWindow& window) {
    if (sprite) window.draw(*sprite);
}

int Boss::getHP() const { return hp; }
void Boss::phaseTwoAttack(Hero& player, vector<Projectile>& projectiles) {
    if (!sprite) return;

    Vector2f bossPos = sprite->getPosition();
    Vector2f playerPos = player.shape.getPosition();

    if (rand() % 2 == 0) {

        Vector2f dir = playerPos - bossPos;
        float len = sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len != 0) dir /= len;
        projectiles.emplace_back(bossPos.x, bossPos.y, dir * 600.f);
    } else {

        for (int i = -1; i <= 1; i++) {
            Vector2f dir = playerPos - bossPos;
            float len = sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len != 0) dir /= len;


            float angleOffset = i * 15.f * (3.14159f / 180.f); 
            float cosA = cos(angleOffset);
            float sinA = sin(angleOffset);

            Vector2f rotatedDir(
                dir.x * cosA - dir.y * sinA,
                dir.x * sinA + dir.y * cosA
            );

            projectiles.emplace_back(bossPos.x, bossPos.y, rotatedDir * 300.f);
        }
    }
}
