#include "bosses.hpp"
#include "hero.hpp"
#include <cmath>
using namespace sf;
using namespace std;
void Boss::phaseTwoAttack(Hero& player, vector<Projectile>& projectiles) {
    if (rand() % 2 == 0) {
        Vector2f laserDir = player.shape.getPosition() - shape.getPosition();
        float len = sqrt(laserDir.x * laserDir.x + laserDir.y * laserDir.y);
        laserDir /= len;
        projectiles.push_back(Projectile(shape.getPosition().x, shape.getPosition().y, laserDir * 600.f));
    } else {
        for (int i = -1; i <= 1; i++) {
            Vector2f flameDir = player.shape.getPosition() - shape.getPosition();
            float len = sqrt(flameDir.x * flameDir.x + flameDir.y * flameDir.y);
            flameDir /= len;
            flameDir.x += i * 100.f;
            projectiles.push_back(Projectile(shape.getPosition().x, shape.getPosition().y, flameDir * 300.f));
        }
    }
}
