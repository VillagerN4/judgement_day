#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "hero.hpp"
#include "bosses.hpp"
#include "projectile.hpp"
#include "crystal.hpp"
#include "ui.hpp"
#include "platform.hpp"

using namespace sf;
using namespace std;

int main() {
    ContextSettings settings;
    settings.antiAliasingLevel = 8;
    Vector2u WINDOW_SIZE(1920, 1080);
    RenderWindow window(VideoMode(WINDOW_SIZE), "Judgement Day", State::Fullscreen, settings);
    window.setFramerateLimit(60);

    Hero player(100.f, WINDOW_SIZE.y - 150.f, 20);
    Boss witch(WINDOW_SIZE.x / 2.f - 30.f, 440.f, Color::Green, 20);
    witch.shape.setSize({60.f, 100.f});

    Crystal leftCrystal(100.f, 150.f);
    Crystal rightCrystal(WINDOW_SIZE.x - 130.f, 150.f);

    vector<Projectile> projectiles;
    Clock clock;
    float shootTimer = 0.f;
    const float shootInterval = 1.f;

    Platform platform(Vector2f(WINDOW_SIZE.x, 50.f), Vector2f(0.f, WINDOW_SIZE.y - 50.f), Color(50, 205, 50));
    UI hpBar(WINDOW_SIZE, player.hp, witch.hp);

    bool onGround = false;
    bool isShooting = false;
    bool playerHit = false;
    float hitTimer = 0.f;
    bool isBossPhaseTwo = false;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        player.handleMovement(deltaTime, 200.f, window.getSize());
        player.handleJumping(deltaTime, WINDOW_SIZE.y - 150.f);
        
        shootTimer += deltaTime;
        if (shootTimer >= shootInterval) {
            shootTimer = 0.f;
            if (leftCrystal.alive) {
                Vector2f dir = player.shape.getPosition() - leftCrystal.shape.getPosition();
                projectiles.push_back(Projectile(leftCrystal.shape.getPosition().x, leftCrystal.shape.getPosition().y, dir * 400.f));
            }
            if (rightCrystal.alive) {
                Vector2f dir = player.shape.getPosition() - rightCrystal.shape.getPosition();
                projectiles.push_back(Projectile(rightCrystal.shape.getPosition().x, rightCrystal.shape.getPosition().y, dir * 400.f));
            }
        }

        for (auto& proj : projectiles) {
            proj.update(deltaTime);
        }

        for (auto i = projectiles.begin(); i != projectiles.end();) {
            if (i->shape.getGlobalBounds().findIntersection(player.shape.getGlobalBounds())) {
                player.hp -= 1;
                playerHit = true;
                hitTimer = 0.2f;
                i = projectiles.erase(i);
            } else {
                ++i;
            }
        }

        if (!leftCrystal.alive || !rightCrystal.alive) {
            witch.shape.setFillColor(Color::Red);
            if (!isBossPhaseTwo) {
                isBossPhaseTwo = true;
            }
        }

        if (isBossPhaseTwo) {
            witch.phaseTwoAttack(player, projectiles);
        }

        if (hitTimer > 0.f) {
            hitTimer -= deltaTime;
            player.shape.setFillColor(Color::Red);
        } else {
            player.shape.setFillColor(Color::Blue);
        }

        hpBar.update(player.hp, witch.hp);

        window.clear();
        window.draw(platform.shape);
        window.draw(witch.shape);
        window.draw(hpBar.hpBarBack);
        window.draw(hpBar.hpBar);
        if (leftCrystal.alive) window.draw(leftCrystal.shape);
        if (rightCrystal.alive) window.draw(rightCrystal.shape);
        
        for (auto& proj : projectiles) {
            proj.shape.setFillColor(Color(rand() % 255, rand() % 255, rand() % 255));
            window.draw(proj.shape);
        }
        
        player.draw(window);
        window.display();
    }

    return 0;
}
