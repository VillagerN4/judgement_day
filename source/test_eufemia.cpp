#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "bosses.hpp"
#include "hero.hpp"
//PLIK ZOSTANIE DODANY DO MAIN LECZ GDY ZROBIONA BĘDZIE JUŻ MAPA
using namespace sf;
using namespace std;

int main() {
    Vector2u WINDOW_SIZE(1920, 1080);
    RenderWindow window(WINDOW_SIZE, "Judgement Day", State::Fullscreen);

    Hero player(100.f, WINDOW_SIZE.y - 150.f, 20);
    Boss witch(835.f, 440.f, Color::Green);
    witch.shape.setSize({60.f, 100.f});

    Crystal leftCrystal(100.f, 150.f);
    Crystal rightCrystal(1790.f, 150.f);

    vector<Projectile> projectiles;
    Clock clock;
    float shootTimer = 0.f;
    const float shootInterval = 1.f;

    RectangleShape platform;
    platform.setSize(Vector2f(WINDOW_SIZE.x, 50.f));
    platform.setPosition(Vector2f(0.f, WINDOW_SIZE.y - 50.f));
    platform.setFillColor(Color::Green);

    bool onGround = false;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        if (Keyboard::isKeyPressed(Keyboard::Key::A))
            player.move(60.f, deltaTime);
        if (Keyboard::isKeyPressed(Keyboard::Key::D))
            player.move(60.f, deltaTime);

        if (player.shape.getPosition().x < 0.f)
            player.shape.setPosition(Vector2f(0.f, player.shape.getPosition().y));
        if (player.shape.getPosition().x + player.shape.getSize().x > WINDOW_SIZE.x)
            player.shape.setPosition(Vector2f(WINDOW_SIZE.x - player.shape.getSize().x, player.shape.getPosition().y));

        if (player.shape.getGlobalBounds().findIntersection(platform.getGlobalBounds())) {
            onGround = true;
            player.shape.setPosition(Vector2f(player.shape.getPosition().x, platform.getPosition().y - player.shape.getSize().y));
        } else {
            onGround = false;
        }
        shootTimer += deltaTime;
        if (shootTimer >= shootInterval) {
            shootTimer = 0.f;
            if (leftCrystal.alive) {
                Vector2f dirToPlayer = player.shape.getPosition() - leftCrystal.shape.getPosition();
                float length = sqrt(dirToPlayer.x * dirToPlayer.x + dirToPlayer.y * dirToPlayer.y);
                dirToPlayer /= length;
                projectiles.emplace_back(leftCrystal.shape.getPosition().x, leftCrystal.shape.getPosition().y, dirToPlayer * 300.f);
            }
            if (rightCrystal.alive) {
                Vector2f dirToPlayer = player.shape.getPosition() - rightCrystal.shape.getPosition();
                float length = sqrt(dirToPlayer.x * dirToPlayer.x + dirToPlayer.y * dirToPlayer.y);
                dirToPlayer /= length;
                projectiles.emplace_back(rightCrystal.shape.getPosition().x, rightCrystal.shape.getPosition().y, dirToPlayer * 300.f);
            }
        }

        for (auto& proj : projectiles) {
            proj.update(deltaTime);
        }

        for (auto i = projectiles.begin(); i != projectiles.end();) {
            if (i->shape.getGlobalBounds().findIntersection(player.shape.getGlobalBounds())) {
                player.hp -= 1;
                i = projectiles.erase(i);
            } else {
                ++i;
            }
        }

        if (!leftCrystal.alive || !rightCrystal.alive) {
            witch.shape.setFillColor(Color::Red);
        }

        window.clear(Color::Black);
        window.draw(player.shape);
        window.draw(witch.shape);
        if (leftCrystal.alive)
            window.draw(leftCrystal.shape);
        if (rightCrystal.alive)
            window.draw(rightCrystal.shape);
        for (auto& proj : projectiles) {
            window.draw(proj.shape);
        }
        window.draw(platform);
        window.display();
    }

    return 0;
}
