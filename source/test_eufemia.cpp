// #include <SFML/Graphics.hpp>
// #include <vector>
// #include <cmath>
// #include <iostream>
// #include <cstdlib>
// #include "hero.hpp"
// #include "bosses.hpp"
// #include "projectile.hpp"
// #include "crystal.hpp"
// #include "ui.hpp"
// #include "platform.hpp"

// using namespace sf;
// using namespace std;

// int main() {
//     ContextSettings settings;
//     settings.antiAliasingLevel = 8;
//     Vector2u WINDOW_SIZE(1920, 1080);
//     RenderWindow window(VideoMode(WINDOW_SIZE), "Judgement Day", State::Fullscreen, settings);
//     window.setFramerateLimit(60);

//     Hero player(100.f, WINDOW_SIZE.y - 150.f, 20);
//     Boss witch(WINDOW_SIZE.x / 2.f - 75.f, 340.f, "assets/textures/entity/boss/eufemia/eufemia.png", 20);
//     Crystal leftCrystal("assets/textures/entity/boss/eufemia/crystal.png", 220.f, 200.f, 120.f);
//     Crystal rightCrystal("assets/textures/entity/boss/eufemia/crystal.png", WINDOW_SIZE.x - 220.f, 200.f, 120.f);

//     vector<Projectile> enemyProjectiles;
//     Clock clock;
//     float shootTimer = 0.f;
//     const float shootInterval = 1.f;

//     Platform platform(Vector2f(WINDOW_SIZE.x, 50.f), Vector2f(0.f, WINDOW_SIZE.y - 50.f), Color(16, 16, 16));

//     RectangleShape leftCrystalHpBack(Vector2f(150.f, 15.f));
//     leftCrystalHpBack.setFillColor(Color(100, 100, 100));
//     leftCrystalHpBack.setPosition(Vector2f(50.f, WINDOW_SIZE.y - 40.f));

//     RectangleShape leftCrystalHp(Vector2f(150.f, 15.f));
//     leftCrystalHp.setFillColor(Color::Cyan);
//     leftCrystalHp.setPosition(leftCrystalHpBack.getPosition());

//     RectangleShape rightCrystalHpBack(Vector2f(150.f, 15.f));
//     rightCrystalHpBack.setFillColor(Color(100, 100, 100));
//     rightCrystalHpBack.setPosition(Vector2f(WINDOW_SIZE.x - 200.f, WINDOW_SIZE.y - 40.f));

//     RectangleShape rightCrystalHp(Vector2f(150.f, 15.f));
//     rightCrystalHp.setFillColor(Color::Cyan);
//     rightCrystalHp.setPosition(rightCrystalHpBack.getPosition());

//     RectangleShape bossHpBack(Vector2f(300.f, 20.f));
//     bossHpBack.setFillColor(Color(100, 100, 100));
//     bossHpBack.setPosition(Vector2f(WINDOW_SIZE.x / 2.f - 150.f, WINDOW_SIZE.y - 40.f));

//     RectangleShape bossHp(Vector2f(300.f, 20.f));
//     bossHp.setFillColor(Color::Magenta);
//     bossHp.setPosition(bossHpBack.getPosition());

//     RectangleShape playerHpBack(Vector2f(200.f, 20.f));
//     playerHpBack.setFillColor(Color(100, 100, 100));
//     playerHpBack.setPosition(Vector2f(20.f, 20.f));

//     RectangleShape playerHpBar(Vector2f(200.f, 20.f));
//     playerHpBar.setFillColor(Color::Blue);
//     playerHpBar.setPosition(playerHpBack.getPosition());

//     bool playerHit = false;
//     float hitTimer = 0.f;
//     bool isBossPhaseTwo = false;

//     Clock phaseTwoClock;
//     bool phaseTwoAttackActive = true;
//     Texture arenaT;
//     if (!arenaT.loadFromFile("assets\\textures\\tile\\eufemia_arena.png", false, IntRect({0, 0}, {910, 512}))){
//        cout << "Menu panorama not found!" << endl;
//     }

//     Sprite arena(arenaT);
//     arena.setPosition({0.f, 0.f});
//     arena.setScale({2.11f, 2.11f});
//     while (window.isOpen()) {
//         float deltaTime = clock.restart().asSeconds();

//         while (const std::optional event = window.pollEvent())
//         {
//             if (event->is<sf::Event::Closed>())
//                 window.close();
//         }
        
//         playerHpBar.setSize(Vector2f(200.f * (float)player.hp / 100.f, 20.f));

//         // if(player.hp <= 0){
//         //     state = GameState::EndScreen;
//         // }else if (witch.getHP())
//         // {
//         //     isWin = true;
//         //     state = Gamestate::EndScreen;
//         // }
        
//         player.handleMovement(deltaTime, 300.f, window.getSize());
//         player.handleJumping(deltaTime, WINDOW_SIZE.y - 150.f);

//         if (Mouse::isButtonPressed(Mouse::Button::Left)) {
//             player.shoot();
//         }

//         shootTimer += deltaTime;
//         if (shootTimer >= shootInterval) {
//             shootTimer = 0.f;

//             if (leftCrystal.isAlive()) {
//                 Vector2f dir = player.shape.getPosition() - leftCrystal.sprite->getPosition();
//                 float len = sqrt(dir.x * dir.x + dir.y * dir.y);
//                 if (len != 0) dir /= len;
//                 enemyProjectiles.emplace_back(leftCrystal.sprite->getPosition().x, leftCrystal.sprite->getPosition().y, dir * 400.f);
//             }
//             if (rightCrystal.isAlive()) {
//                 Vector2f dir = player.shape.getPosition() - rightCrystal.sprite->getPosition();
//                 float len = sqrt(dir.x * dir.x + dir.y * dir.y);
//                 if (len != 0) dir /= len;
//                 enemyProjectiles.emplace_back(rightCrystal.sprite->getPosition().x, rightCrystal.sprite->getPosition().y, dir * 400.f);
//             }
//         }

//         player.updateProjectiles2(deltaTime);

//         for (auto& proj : enemyProjectiles) {
//             proj.update(deltaTime);
//         }

//         for (auto i = enemyProjectiles.begin(); i != enemyProjectiles.end();) {
//             if (i->shape.getGlobalBounds().findIntersection(player.shape.getGlobalBounds())) {
//                 player.hp -= 1;
//                 playerHit = true;
//                 hitTimer = 0.2f;
//                 i = enemyProjectiles.erase(i);
//             } else {
//                 ++i;
//             }
//         }

//         for (auto i = player.projectiles.begin(); i != player.projectiles.end();) {
//             bool erased = false;

//             if (leftCrystal.isAlive() && i->shape.getGlobalBounds().findIntersection(leftCrystal.sprite->getGlobalBounds())) {
//                 leftCrystal.takeDamage(1);
//                 i = player.projectiles.erase(i);
//                 erased = true;
//             }
//             else if (rightCrystal.isAlive() && i->shape.getGlobalBounds().findIntersection(rightCrystal.sprite->getGlobalBounds())) {
//                 rightCrystal.takeDamage(1);
//                 i = player.projectiles.erase(i);
//                 erased = true;
//             }
//             else if (isBossPhaseTwo && witch.isAlive() && i->shape.getGlobalBounds().findIntersection(witch.sprite->getGlobalBounds())) {
//                 witch.takeDamage(1);
//                 i = player.projectiles.erase(i);
//                 erased = true;
//             }

//             if (!erased) {
//                 ++i;
//             }
//         }

//         if (!leftCrystal.isAlive() && !rightCrystal.isAlive()) {
//             if (!isBossPhaseTwo) {
//                 isBossPhaseTwo = true;
//                 phaseTwoClock.restart();
//                 phaseTwoAttackActive = true;
//             }
//         }

//         if (isBossPhaseTwo && witch.isAlive()) {
//             float phaseTime = phaseTwoClock.getElapsedTime().asSeconds();
//             if (phaseTime >= 3.f && phaseTime < 6.f) {
//                 phaseTwoAttackActive = false;
//             } else if (phaseTime >= 6.f) {
//                 phaseTwoClock.restart();
//                 phaseTwoAttackActive = true;
//             }

//             if (phaseTwoAttackActive) {
//                 witch.phaseTwoAttack(player, enemyProjectiles);
//             }
//         }

//         if (hitTimer > 0.f) {
//             hitTimer -= deltaTime;
//             player.shape.setFillColor(Color::Red);
//         } else {
//             player.shape.setFillColor(Color::Blue);
//         }

//         leftCrystal.update(deltaTime);
//         rightCrystal.update(deltaTime);

//         leftCrystalHp.setSize(Vector2f(150.f * (float)leftCrystal.getHP() / 5.f, 15.f));
//         rightCrystalHp.setSize(Vector2f(150.f * (float)rightCrystal.getHP() / 5.f, 15.f));
//         bossHp.setSize(Vector2f(300.f * (float)witch.getHP() / 20.f, 20.f));

//         window.clear();
//         window.draw(arena);
//         window.draw(platform.shape);
//         window.draw(playerHpBack);
//         window.draw(playerHpBar);
//         if (witch.sprite) window.draw(*witch.sprite);

//         if (leftCrystal.isAlive()) window.draw(*leftCrystal.sprite);
//         if (rightCrystal.isAlive()) window.draw(*rightCrystal.sprite);

//         for (auto& proj : player.projectiles) {
//             proj.shape.setFillColor(Color::Yellow);
//             window.draw(proj.shape);
//         }
//         for (auto& proj : enemyProjectiles) {
//             proj.shape.setFillColor(Color::Red);
//             window.draw(proj.shape);
//         }

//         player.draw(window);

//         window.draw(leftCrystalHpBack);
//         window.draw(leftCrystalHp);

//         window.draw(rightCrystalHpBack);
//         window.draw(rightCrystalHp);

//         window.draw(bossHpBack);
//         window.draw(bossHp);

//         window.display();
//     }

//     return 0;
// }
